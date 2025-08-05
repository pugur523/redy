// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <cstddef>
#include <cstdint>
#include <utility>

#include "unicode/base/unicode_util.h"

namespace unicode {

constexpr uint32_t kInvalidUnicodePoint = 0xFFFD;

// returns (codepoint, byte_count). if invalid, returns (0xFFFD, 1).
std::pair<uint32_t, std::size_t> decode_utf8(const char* input,
                                             std::size_t size) {
  if (size == 0) {
    return {kInvalidUnicodePoint, 0};
  }

  const auto* bytes = reinterpret_cast<const unsigned char*>(input);
  unsigned char b0 = bytes[0];

  // ascii fast path
  if (is_ascii(b0)) {
    return {b0, 1};
  }

  // determine sequence length
  unsigned char mask = (b0 >> 3) == 0x1E  ? 0xF8
                       : (b0 >> 4) == 0xE ? 0xF0
                       : (b0 >> 5) == 0x6 ? 0xE0
                                          : 0xFF;
  std::size_t len = mask == 0xE0 ? 2 : mask == 0xF0 ? 3 : mask == 0xF8 ? 4 : 1;
  if (len == 1 || size < len) {
    return {kInvalidUnicodePoint, 1};
  }

  // check continuation bytes
  for (std::size_t i = 1; i < len; ++i) {
    if ((bytes[i] & 0xC0) != 0x80) {
      return {kInvalidUnicodePoint, 1};
    }
  }

  uint32_t cp = 0;
  switch (len) {
    case 2:
      cp = ((b0 & 0x1F) << 6) | (bytes[1] & 0x3F);
      if (cp < 0x80) {
        return {kInvalidUnicodePoint, 1};
      }
      break;
    case 3:
      cp = ((b0 & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
      if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) {
        return {kInvalidUnicodePoint, 1};
      }
      break;
    case 4:
      cp = ((b0 & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
           ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
      if (cp < 0x10000 || cp > 0x10FFFF) {
        return {kInvalidUnicodePoint, 1};
      }
      break;
    default: return {kInvalidUnicodePoint, 1};
  }
  return {cp, len};
}

}  // namespace unicode
