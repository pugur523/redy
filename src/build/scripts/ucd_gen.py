#!/usr/bin/env python3

# Copyright 2025 pugur
# This source code is licensed under the Apache License, Version 2.0
# which can be found in the LICENSE file.

import os
import time
import urllib.request
import hashlib

from build_util import (
    project_src_dir,
    project_resources_dir,
)

ucd_raw_dir = os.path.join(project_resources_dir, "ucd_raw")
ucd_generated_dir = os.path.join(
    os.path.join(project_src_dir, "unicode", "base", "ucd")
)
source_path = os.path.join(ucd_generated_dir, "unicode_data.cc")
checksum_path = os.path.join(ucd_raw_dir, ".checksum")

kBaseUrl = "https://www.unicode.org/Public/UCD/latest/ucd/"
kFiles = {
    "DerivedCoreProperties.txt": ["XID_Start", "XID_Continue"],
    "PropList.txt": ["White_Space"],
    "UnicodeData.txt": [
        "General_Category"
    ],  # for Nd (decimal numbers), Lu/Ll/Lt (letters)
}


def calculate_checksum(file_paths):
    sha256 = hashlib.sha256()
    for path in file_paths:
        with open(path, "rb") as f:
            while chunk := f.read(4096):
                sha256.update(chunk)
    return sha256.hexdigest()


def get_file_checksum(file_path):
    sha256 = hashlib.sha256()
    with open(file_path, "rb") as f:
          while chunk := f.read(4096):
                sha256.update(chunk)
    return sha256.hexdigest()


def should_skip_generation(raw_files):
    if not os.path.exists(source_path) or not os.path.exists(checksum_path):
        return False

    try:
        with open(checksum_path, "r", encoding="utf-8") as f:
            saved_checksum = f.read().strip()
        current_cc_checksum = get_file_checksum(source_path)
        current_raw_checksum = calculate_checksum(raw_files)

        return saved_checksum == f"{current_cc_checksum}\n{current_raw_checksum}"
    except (IOError, FileNotFoundError):
        return False


def download_ucd_file(file_name, dest_path):
    url = kBaseUrl + file_name
    print(f"Downloading {url}")
    os.makedirs(ucd_raw_dir, exist_ok=True)
    urllib.request.urlretrieve(url, dest_path)


def parse_prop_list(file_path, target_properties):
    """Parse PropList.txt or DerivedCoreProperties.txt format"""
    properties_data = {prop: [] for prop in target_properties}

    with open(file_path, encoding="utf-8") as f:
        for line in f:
            line = line.split("#")[0].strip()
            if not line or ";" not in line:
                continue
            range_part, prop = [s.strip() for s in line.split(";", 1)]
            if prop not in target_properties:
                continue

            if ".." in range_part:
                start, end = [int(x, 16) for x in range_part.split("..")]
            else:
                start = end = int(range_part, 16)
            properties_data[prop].append((start, end))

    return {prop: compress_ranges(ranges) for prop, ranges in properties_data.items()}


def parse_unicode_data(file_path):
    categories_data = {
        "Nd": [],  # decimal numbers
        "Lu": [],  # uppercase letters
        "Ll": [],  # lowercase letters
        "Lt": [],  # titlecase letters
        "Lm": [],  # modifier letters
        "Lo": [],  # other letters
    }

    with open(file_path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            fields = line.split(";")
            if len(fields) < 3:
                continue

            codepoint = int(fields[0], 16)
            category = fields[2]

            if category in categories_data:
                categories_data[category].append((codepoint, codepoint))

    return {cat: compress_ranges(ranges) for cat, ranges in categories_data.items()}


def compress_ranges(ranges):
    if not ranges:
        return []
    ranges.sort()
    compressed = [ranges[0]]
    for start, end in ranges[1:]:
        last_start, last_end = compressed[-1]
        if start <= last_end + 1:
            compressed[-1] = (last_start, max(last_end, end))
        else:
            compressed.append((start, end))
    return compressed


def emit_range_array(varname, ranges):
    lines = [f"constexpr UnicodeRange {varname}[] = {{"]
    for start, end in ranges:
        lines.append(f"  {{ 0x{start:04X}, 0x{end:04X} }},")
    lines.append("};")
    lines.append(
        f"constexpr std::size_t {varname}Count = sizeof({varname}) / sizeof(UnicodeRange);"
    )
    return "\n".join(lines)


def generate_cc(all_data):
    os.makedirs(ucd_generated_dir, exist_ok=True)

    with open(source_path, "w", encoding="utf-8") as cc:
        cc.write(
            f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

// clang-format off
// NOLINTBEGIN

#include "unicode/base/ucd/unicode_data.h"

namespace unicode {{

// xid properties
{emit_range_array("kXIDStart", all_data["XID_Start"])}

{emit_range_array("kXIDContinue", all_data["XID_Continue"])}

// whitespace property
{emit_range_array("kWhiteSpace", all_data["White_Space"])}

// general categories
{emit_range_array("kDecimalNumber", all_data["Nd"])}

{emit_range_array("kUppercaseLetter", all_data["Lu"])}

{emit_range_array("kLowercaseLetter", all_data["Ll"])}

{emit_range_array("kTitlecaseLetter", all_data["Lt"])}

{emit_range_array("kModifierLetter", all_data["Lm"])}

{emit_range_array("kOtherLetter", all_data["Lo"])}

// NOLINTEND
// clang-format on

}}  // namespace unicode
"""
        )


def download_ucd_raw_if_not_exists(file):
    path = os.path.join(ucd_raw_dir, file)
    if not os.path.exists(path):
        download_ucd_file(file, path)
    return path


def main():
    start_time = time.time()
    all_data = {}

    # download batch
    derived_path = download_ucd_raw_if_not_exists("DerivedCoreProperties.txt")
    prop_path = download_ucd_raw_if_not_exists("PropList.txt")
    unicode_path = download_ucd_raw_if_not_exists("UnicodeData.txt")

    raw_files = [derived_path, prop_path, unicode_path]
    if should_skip_generation(raw_files):
        print("checksums match. skipping code generation.")
        return

    derived_data = parse_prop_list(derived_path, ["XID_Start", "XID_Continue"])
    all_data.update(derived_data)

    prop_data = parse_prop_list(prop_path, ["White_Space"])
    all_data.update(prop_data)

    unicode_data = parse_unicode_data(unicode_path)
    all_data.update(unicode_data)

    generate_cc(all_data)

    end_time = time.time()
    total_sec = end_time - start_time
    minutes = int(total_sec // 60)
    seconds = int(total_sec % 60)

    # save checksums after successful generation
    with open(checksum_path, "w", encoding="utf-8") as f:
        cc_checksum = get_file_checksum(source_path)
        raw_checksum = calculate_checksum(raw_files)
        f.write(f"{cc_checksum}\n{raw_checksum}")

    print(
        f"done: generated unicode_data.cc in {minutes} min {seconds} sec."
    )


if __name__ == "__main__":
    main()
