// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/base/utf8_decode.h"

#include <cstddef>
#include <cstdint>
#include <utility>

#include "unicode/base/unicode_util.h"

namespace unicode {

std::pair<uint32_t, std::size_t> decode_utf8(const char* input,
                                             std::size_t size) {
  if (size == 0) {
    return {kInvalidUnicodePoint, 0};
  }

  const auto* bytes = reinterpret_cast<const unsigned char*>(input);
  const unsigned char b0 = bytes[0];

  // ascii fast path
  if (is_ascii(b0)) {
    return {b0, 1};
  }

  const uint8_t len = utf8_sequence_length(b0);

  if (len == 0 || size < len) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  }

  // check continuation bytes
  uint32_t continuation_mask = 0;
  for (std::size_t i = 1; i < len; ++i) {
    continuation_mask |= (bytes[i] & 0xC0) ^ 0x80;
  }

  if (continuation_mask != 0) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  }

  // decode codepoint with optimized bit operations
  uint32_t cp;
  switch (len) {
    case 2:
      cp = ((b0 & 0x1F) << 6) | (bytes[1] & 0x3F);
      // check for overlong encoding
      if (cp < 0x80) [[unlikely]] {
        return {kInvalidUnicodePoint, 1};
      }
      break;

    case 3:
      cp = ((b0 & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
      // check for overlong encoding and surrogate pairs
      if (cp < 0x800 || (cp >= 0xD800 && cp <= 0xDFFF)) [[unlikely]] {
        return {kInvalidUnicodePoint, 1};
      }
      break;

    case 4:
      cp = ((b0 & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
           ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
      // check for overlong encoding and valid unicode range
      if (cp < 0x10000 || cp > 0x10FFFF) [[unlikely]] {
        return {kInvalidUnicodePoint, 1};
      }
      break;
  }

  return {cp, len};
}

}  // namespace unicode
