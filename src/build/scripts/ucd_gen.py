#!/usr/bin/env python3

import urllib.request
import os

from build_util import (
    project_src_dir,
    project_resources_dir,
)

ucd_raw_dir = os.path.join(project_resources_dir, "ucd_raw")
ucd_generated_dir = os.path.join(os.path.join(project_src_dir, "core", "base", "utf", "ucd"))
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

def parse_and_compress_ucd(file_path, target_property):
    ranges = []
    with open(file_path, encoding="utf-8") as f:
        for line in f:
            line = line.split("#")[0].strip()
            if not line or ";" not in line:
                continue
            range_part, prop = [s.strip() for s in line.split(";", 1)]
            if prop != target_property:
                continue
            if ".." in range_part:
                start, end = [int(x, 16) for x in range_part.split("..")]
            else:
                start = end = int(range_part, 16)
            ranges.append((start, end))
    return compress_ranges(ranges)

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
    lines.append(f"constexpr std::size_t {varname}Count = sizeof({varname}) / sizeof(UnicodeRange);")
    return "\n".join(lines)


def generate_cc_files(xid_start, xid_continue):
    os.makedirs(ucd_generated_dir, exist_ok=True)

    with open(header_path, "w", encoding="utf-8") as h:
        h.write(f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_UTF_UCD_UNICODE_DATA_H_
#define CORE_BASE_UTF_UCD_UNICODE_DATA_H_

#include <cstddef>
#include <cstdint>

namespace core {{

struct UnicodeRange {{
  uint32_t start;
  uint32_t end;
}};

extern const UnicodeRange kXIDStart[];
extern const std::size_t kXIDStartCount;
extern const UnicodeRange kXIDContinue[];
extern const std::size_t kXIDContinueCount;

constexpr bool is_in_ranges(const UnicodeRange* ranges, std::size_t count, uint32_t codepoint) {{
  std::size_t low = 0;
  std::size_t high = count;
  while (low < high) {{
    std::size_t mid = (low + high) / 2;
    if (codepoint < ranges[mid].start) {{
      high = mid;
    }} else if (codepoint > ranges[mid].end) {{
      low = mid + 1;
    }} else {{
      return true;
    }}
  }}
  return false;
}}

inline constexpr bool is_xid_start(uint32_t codepoint) {{
  return is_in_ranges(kXIDStart, kXIDStartCount, codepoint);
}}

inline constexpr bool is_xid_continue(uint32_t codepoint) {{
  return is_in_ranges(kXIDContinue, kXIDContinueCount, codepoint);
}}

}}  // namespace core

#endif  // CORE_BASE_UTF_UCD_UNICODE_DATA_H_
""")

    with open(source_path, "w", encoding="utf-8") as cc:
        cc.write(f"""\
// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/base/utf/ucd/unicode_data.h"

namespace core {{

{emit_range_array("kXIDStart", xid_start)}

{emit_range_array("kXIDContinue", xid_continue)}

}}  // namespace core

""")


def main():
    file_name = "DerivedCoreProperties.txt"
    file_path = os.path.join(ucd_raw_dir, file_name)

    if not os.path.exists(file_path):
        download_file(file_name, file_path)

    xid_start = parse_and_compress_ucd(file_path, "XID_Start")
    xid_continue = parse_and_compress_ucd(file_path, "XID_Continue")

    generate_cc_files(xid_start, xid_continue)
    print("Done: generated unicode_data.h / unicode_data.cc")


if __name__ == "__main__":
    main()
