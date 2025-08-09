// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/decoder.h"

#include <cstddef>
#include <cstdint>
#include <utility>

#include "core/base/string_util.h"
#include "core/check.h"
#include "unicode/base/unicode_util.h"

namespace unicode {

std::pair<char32_t, std::size_t> Utf8Decoder::decode(const char8_t* input,
                                                     std::size_t size) const {
  DCHECK_GT(size, 0);

  const auto* const bytes = reinterpret_cast<const unsigned char*>(input);
  const unsigned char b0 = bytes[0];

  const uint8_t len = utf8_sequence_length(b0);

  if (len == 0 || size < len) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  } else if (len == 1) [[likely]] {
    // ascii fast path
    return {b0, 1};
  }

  if (!is_valid_continuation_sequence(bytes, len)) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  }

  char32_t cp;
  switch (len) {
    case 2: cp = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F); break;
    case 3:
      cp = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) |
           (bytes[2] & 0x3F);
      break;
    case 4:
      cp = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
           ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
      break;
    default: return {kInvalidUnicodePoint, 1};
  }

  if (!is_valid_codepoint(cp, len)) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  }

  return {cp, len};
}

}  // namespace unicode
