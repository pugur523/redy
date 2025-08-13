#!/usr/bin/env python3

# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

import hashlib
import os
import re
import sys
import time
import tomllib
from typing import Dict, Any, Set

from build_util import (
    project_src_dir,
    project_resources_dir,
)

i18n_data_dir = os.path.join(project_resources_dir, "i18n")
i18n_generated_dir = os.path.join(os.path.join(project_src_dir, "i18n", "base", "data"))
lang_ids_header_path = os.path.join(i18n_generated_dir, "language_id.h")
tr_keys_header_path = os.path.join(i18n_generated_dir, "translation_key.h")
tr_impl_header_path = os.path.join(i18n_generated_dir, "translation_impl.h")
default_lang_name = "en_uk"


def calculate_dir_hash(dir_path: str, ext: str) -> str:
    """recursively calculate the sha256 hash of all files in a directory."""
    hasher = hashlib.sha256()
    file_paths = []
    for root, _, files in os.walk(dir_path):
        for file in files:
            if file.endswith(ext):
                file_paths.append(os.path.join(root, file))

    file_paths.sort()  # ensure consistent order

    for file_path in file_paths:
        with open(file_path, "rb") as f:
            while chunk := f.read(4096):
                hasher.update(chunk)
    return hasher.hexdigest()


def to_enum_id(identifier: str):
    identifier = identifier.lower()

    parts = re.split(r"[-_\\.]", identifier)

    pascal_case = "".join(part.capitalize() for part in parts)

    return f"k{pascal_case}"


def flatten_dict(
    d: Dict[str, Any], parent_key: str = "", sep: str = "."
) -> Dict[str, str]:
    """convert nested dict into flat key structure"""
    items = []
    for k, v in d.items():
        new_key = f"{parent_key}{sep}{k}" if parent_key else k
        if isinstance(v, dict):
            items.extend(flatten_dict(v, new_key, sep=sep).items())
        else:
            items.append((new_key, str(v)))
    return dict(items)


def parse_lang_dir(lang_dir: str) -> Dict[str, str]:
    """parse all toml files in a language directory and merge them"""
    merged_data = {}

    if not os.path.exists(lang_dir):
        return merged_data

    for filename in os.listdir(lang_dir):
        if filename.endswith(".toml"):
            category = os.path.splitext(filename)[0]
            file_path = os.path.join(lang_dir, filename)

            try:
                with open(file_path, "rb") as f:
                    data = tomllib.load(f)
                    # add category prefix to keys
                    flattened = flatten_dict(data)
                    for key, value in flattened.items():
                        full_key = f"{category}.{key}"
                        merged_data[full_key] = value
            except Exception as e:
                print(f"error parsing {file_path}: {e}")

    return merged_data


def parse_legacy_file(file_path: str) -> Dict[str, str]:
    """parse legacy single toml file format"""
    try:
        with open(file_path, "rb") as f:
            data = tomllib.load(f)
            return flatten_dict(data)
    except Exception as e:
        print(f"error parsing {file_path}: {e}")
        return {}


def parse_dir(dir_path: str) -> Dict[str, Dict[str, str]]:
    """parse i18n data supporting both directory and file structures"""
    i18n_data = {}

    if not os.path.exists(dir_path):
        print(f"warning: i18n data directory not found at {dir_path}")
        return i18n_data

    for item in os.listdir(dir_path):
        item_path = os.path.join(dir_path, item)

        if os.path.isdir(item_path):
            # new directory-based structure: lang_name/category.toml
            lang_name = item
            lang_data = parse_lang_dir(item_path)
            if lang_data:
                i18n_data[lang_name] = lang_data
        elif item.endswith(".toml"):
            # legacy single file structure: lang_name.toml
            lang_name = os.path.splitext(item)[0]
            lang_data = parse_legacy_file(item_path)
            if lang_data:
                i18n_data[lang_name] = lang_data

    return i18n_data


def validate_and_warn_missing_keys(data: Dict[str, Dict[str, str]], default_lang: str):
    """check for missing keys and print warnings"""
    if default_lang not in data:
        print(f"error: default language '{default_lang}' not found")
        return

    default_keys = set(data[default_lang].keys())

    for lang_name, lang_data in data.items():
        if lang_name == default_lang:
            continue

        missing_keys = default_keys - set(lang_data.keys())
        if missing_keys:
            print(
                f"warning: language '{lang_name}' missing keys: {sorted(missing_keys)}"
            )
            # Add fallback keys
            for key in missing_keys:
                lang_data[key] = data[default_lang][key]
                # too noisy
                # print(f"  -> fallback '{key}' to default value")


def build_string_table(
    data: Dict[str, Dict[str, str]],
) -> tuple[Dict[str, int], list[str]]:
    """build deduplicated string table for memory efficiency"""
    string_to_index = {}
    string_table = []

    string_table.append("unknown_key")
    string_to_index["unknown_key"] = 0

    # collect all unique strings
    all_strings = set()
    for lang_data in data.values():
        all_strings.update(lang_data.values())

    # sort for deterministic output
    for string in sorted(all_strings):
        if string == "unknown_key":
            continue

        string_to_index[string] = len(string_table)
        string_table.append(string)

    return string_to_index, string_table


def generate_all(data: Dict[str, Dict[str, str]]):
    validate_and_warn_missing_keys(data, default_lang_name)

    # get all translation keys from default language
    all_keys = list(data[default_lang_name].keys())

    # build optimized string table
    string_to_index, string_table = build_string_table(data)

    languages = list(data.keys())

    generate_lang_ids_header(languages)
    generate_translation_keys_header(all_keys)
    generate_translation_impl_header(data, all_keys, string_to_index, string_table)


def generate_lang_ids_header(languages: list):
    languages_str = "\n"
    for idx, lang in enumerate(languages):
        languages_str += f"  {to_enum_id(lang)} = {idx},\n"
    languages_str += f"\n  kDefault = {to_enum_id(default_lang_name)},\n"

    with open(lang_ids_header_path, "w", encoding="utf-8") as h:
        h.write(
            f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

// clang-format off
// NOLINTBEGIN

#ifndef I18N_BASE_DATA_LANGUAGE_ID_H_
#define I18N_BASE_DATA_LANGUAGE_ID_H_

#include <cstddef>
#include <cstdint>

namespace i18n {{

enum class LanguageId : uint8_t {{{languages_str}}};

constexpr std::size_t kDefaultLanguageIndex =
    static_cast<std::size_t>(LanguageId::kDefault);

}}  // namespace i18n

// NOLINTEND
// clang-format on

#endif  // I18N_BASE_DATA_LANGUAGE_ID_H_
"""
        )


def generate_translation_keys_header(keys: list):
    keys_count = len(keys) + 1  # +1 for kUnknown

    # choose optimal integer type based on key count
    if keys_count <= 256:
        derived_type = "uint8_t"
    elif keys_count <= 65536:
        derived_type = "uint16_t"
    else:
        derived_type = "uint32_t"

    keys_str = "\n"
    keys_str += "  kUnknown = 0,\n"
    for idx, key in enumerate(keys, start=1):
        keys_str += f"  {to_enum_id(key)} = {idx},\n"

    with open(tr_keys_header_path, "w", encoding="utf-8") as h:
        h.write(
            f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

// clang-format off
// NOLINTBEGIN

#ifndef I18N_BASE_DATA_TRANSLATION_KEY_H_
#define I18N_BASE_DATA_TRANSLATION_KEY_H_

#include <cstddef>
#include <cstdint>

#include "i18n/base/translation_util.h"

namespace i18n {{

enum class TranslationKey : {derived_type} {{{keys_str}}};

constexpr std::size_t kTranslationKeyCount = {keys_count};

}}  // namespace i18n

// NOLINTEND
// clang-format on

#endif  // I18N_BASE_DATA_TRANSLATION_KEY_H_
"""
        )


def generate_translation_impl_header(
    data: Dict[str, Dict[str, str]],
    all_keys: list,
    string_to_index: Dict[str, int],
    string_table: list[str],
):
    """generate memory-efficient translation implementation using string table"""

    # generate string table
    string_table_str = ""
    for string in string_table:
        escaped_string = (
            string.replace('"', '\\"').replace("\\", "\\\\").replace("\n", "\\n")
        )
        string_table_str += f'    "{escaped_string}",\n'

    # generate translation tables (indices into string table)
    lang_tables_str = ""
    for lang_name, lang_data in data.items():
        lang_id = to_enum_id(lang_name)
        table_data = ""

        table_data += f"    0,  // for TranslationKey::kUnknown (should not be used)\n"
        for key in all_keys:
            value = lang_data.get(key, "")
            string_index = string_to_index[value]
            table_data += f"    {string_index},  // {key}\n"

        lang_tables_str += f"constexpr std::array<uint16_t, kTranslationKeyCount> {lang_id}Indices = {{\n{table_data}}};\n\n"
    lang_tables_str = lang_tables_str.removesuffix("\n\n")

    # generate language table array
    lang_array_str = ""
    for lang_name in data.keys():
        lang_id = to_enum_id(lang_name)
        lang_array_str += f"    {lang_id}Indices.data(),\n"

    # generate if constexpr chain for string-to-enum conversion
    conversion_chain = ""
    for idx, key in enumerate(all_keys):
        enum_name = to_enum_id(key)
        if idx == 0:
            conversion_chain += f'  if constexpr (str_equal<key, "{key}">()) {{\n'
        else:
            conversion_chain += (
                f'  }} else if constexpr (str_equal<key, "{key}">()) {{\n'
            )
        conversion_chain += f"    return TranslationKey::{enum_name};\n"

    # add final else clause for compile-time error
    conversion_chain += """  } else {
    // force compilation error for unknown keys
    static_assert(false, "unknown translation key");
    return TranslationKey::kUnknown; // never reached
  }"""

    with open(tr_impl_header_path, "w", encoding="utf-8") as h:
        h.write(
            f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

// clang-format off
// NOLINTBEGIN

#ifndef I18N_BASE_DATA_TRANSLATION_IMPL_H_
#define I18N_BASE_DATA_TRANSLATION_IMPL_H_

#include <array>

#include "i18n/base/data/language_id.h"
#include "i18n/base/data/translation_key.h"

namespace i18n {{

// deduplicated string table for memory efficiency
constexpr std::array<const char*, {len(string_table)}> kStringTable = {{
{string_table_str}}};

// translation index tables (indices into kStringTable)
{lang_tables_str}

// array of translation tables indexed by LanguageId
constexpr std::array kTranslationTables = {{
{lang_array_str}}};

// convert string literal to TranslationKey at compile time
template <FixedString key>
consteval TranslationKey to_translation_key() {{
{conversion_chain}
}}

// NOLINTEND
// clang-format on

}}  // namespace i18n

#endif  // I18N_BASE_DATA_TRANSLATION_IMPL_H_
"""
        )


def main():
    start_time = time.time()
    cache_file = os.path.join(i18n_generated_dir, ".checksum")
    current_toml_hash = calculate_dir_hash(i18n_data_dir, ".toml")
    current_cc_hash_before_gen = calculate_dir_hash(i18n_generated_dir, ".h")

    if os.path.exists(cache_file):
        with open(cache_file, "r") as f:
            last_toml_hash = f.readline().strip()
            last_cc_hash = f.readline().strip()
            if (
                last_toml_hash == current_toml_hash
                and last_cc_hash == current_cc_hash_before_gen
                and os.path.exists(tr_impl_header_path)
                and os.path.exists(tr_keys_header_path)
                and os.path.exists(lang_ids_header_path)
            ):
                print("i18n data has not changed. skipping code generation.")
                return 0
    print("parsing i18n data...")
    data = parse_dir(i18n_data_dir)

    if not data:
        print("no i18n data found. exiting.")
        return 0

    if default_lang_name not in data:
        print(f"error: default language '{default_lang_name}' not found in data")
        return 1

    print("generating c++ code...")
    generate_all(data)
    current_cc_hash_after_gen = calculate_dir_hash(i18n_generated_dir, ".h")

    with open(cache_file, "w") as f:
        f.write(current_toml_hash)
        f.write("\n")
        f.write(current_cc_hash_after_gen)
        f.write("\n")

    # print memory usage statistics
    total_strings = sum(len(lang_data) for lang_data in data.values())
    unique_strings = len(
        set(value for lang_data in data.values() for value in lang_data.values())
    )
    dedup_ratio = (
        (total_strings - unique_strings) / total_strings * 100
        if total_strings > 0
        else 0
    )

    end_time = time.time()
    total_sec = end_time - start_time
    minutes = int(total_sec // 60)
    seconds = int(total_sec % 60)

    print(f"done: parsed and generated i18n data in {minutes} min {seconds} sec.")
    print(f"  languages: {len(data)}")
    print(f"  translation keys: {len(data[default_lang_name])}")
    print(f"  total string entries: {total_strings}")
    print(f"  unique strings: {unique_strings}")
    print(f"  deduplication savings: {dedup_ratio:.1f}%")
    print(f"  new hash: {current_cc_hash_after_gen}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
