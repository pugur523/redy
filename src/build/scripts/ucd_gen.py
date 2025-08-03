#!/usr/bin/env python3

import urllib.request
import os

from build_util import (
    project_src_dir,
    project_resources_dir,
)

ucd_raw_dir = os.path.join(project_resources_dir, "ucd_raw")
ucd_generated_dir = os.path.join(os.path.join(project_src_dir, "build", "ucd"))
header_path = os.path.join(ucd_generated_dir, "unicode_data.h")
source_path = os.path.join(ucd_generated_dir, "unicode_data.cc")

kBaseUrl = "https://www.unicode.org/Public/UCD/latest/ucd/"
kFiles = {
    "DerivedCoreProperties.txt": "XID_Start_and_Continue",
}

def download_file(file_name, dest_path):
    url = kBaseUrl + file_name
    print(f"Downloading {url}")
    os.makedirs(ucd_raw_dir, exist_ok=True)
    urllib.request.urlretrieve(url, dest_path)


def extract_property_ranges(prop_name, file_path):
    ranges = []
    with open(file_path, encoding="utf-8") as f:
        for line in f:
            line = line.split("#")[0].strip()
            if not line or ";" not in line:
                continue
            range_part, prop = [s.strip() for s in line.split(";", 1)]
            if prop != prop_name:
                continue
            if ".." in range_part:
                start, end = [int(x, 16) for x in range_part.split("..")]
            else:
                start = end = int(range_part, 16)
            ranges.append((start, end))
    return ranges



def emit_range_array(varname, ranges):
    lines = [f"constexpr UnicodeRange {varname}[] = {{"]
    for start, end in ranges:
        lines.append(f"  {{0x{start:04X}, 0x{end:04X}}},")
    lines.append("};\n")
    return "\n".join(lines)


def generate_cc_files(xid_start, xid_continue):
    os.makedirs(ucd_generated_dir, exist_ok=True)

    with open(header_path, "w", encoding="utf-8") as h:
        h.write(f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef BUILD_UCD_UNICODE_DATA_H_
#define BUILD_UCD_UNICODE_DATA_H_

#include <cstddef>
#include <cstdint>

struct UnicodeRange {{
  uint32_t start;
  uint32_t end;
}};

extern const UnicodeRange kXIDStart[];
extern const std::size_t kXIDStartCount;
extern const UnicodeRange kXIDContinue[];
extern const std::size_t kXIDContinueCount;

bool is_xid_start(uint32_t codepoint);
bool is_xid_continue(uint32_t codepoint);

#endif  // BUILD_UCD_UNICODE_DATA_H_
""")

    with open(source_path, "w", encoding="utf-8") as cc:
        cc.write(f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "build/ucd/unicode_data.h"

{emit_range_array("kXIDStart", xid_start)}
const std::size_t kXIDStartCount = sizeof(kXIDStart) / sizeof(UnicodeRange);
{emit_range_array("kXIDContinue", xid_continue)}
const std::size_t kXIDContinueCount = sizeof(kXIDContinue) / sizeof(UnicodeRange);
bool is_in_ranges(const UnicodeRange* ranges, std::size_t count, uint32_t codepoint) {{
  for (std::size_t i = 0; i < count; ++i) {{
    if (codepoint >= ranges[i].start && codepoint <= ranges[i].end) {{
      return true;
    }}
  }}
  return false;
}}

bool is_xid_start(uint32_t codepoint) {{
  return is_in_ranges(kXIDStart, kXIDStartCount, codepoint);
}}

bool is_xid_continue(uint32_t codepoint) {{
  return is_in_ranges(kXIDContinue, kXIDContinueCount, codepoint);
}}
""")


def main():
    file_name = "DerivedCoreProperties.txt"
    file_path = os.path.join(ucd_raw_dir, file_name)
    
    if os.path.exists(file_path):
        print(f"{file_path} found; skipped download")
    else:
        download_file(file_name, file_path)
    
    if os.path.exists(header_path) and os.path.exists(source_path):
        print(f"{file_path} found; skipped generate")
    else:
        xid_start = extract_property_ranges("XID_Start", file_path)
        xid_continue = extract_property_ranges("XID_Continue", file_path)
        generate_cc_files(xid_start, xid_continue)
        print("Done: generated unicode_data.h / unicode_data.cc")


if __name__ == "__main__":
    main()
