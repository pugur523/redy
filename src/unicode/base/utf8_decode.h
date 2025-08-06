// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_BASE_UTF8_DECODE_H_
#define UNICODE_BASE_UTF8_DECODE_H_

#include <cstddef>
#include <cstdint>
#include <utility>

#include "unicode/base/unicode_export.h"

namespace unicode {

constexpr const uint32_t kInvalidUnicodePoint = 0xFFFD;

// returns (codepoint, byte_count). if invalid, returns (0xFFFD, 1).
UNICODE_EXPORT std::pair<uint32_t, std::size_t> decode_utf8(const char* input,
                                                            std::size_t size);

// consumes one codepoint from utf8 string, returns (codepoint, next_ptr)
inline std::pair<uint32_t, const char*> next_codepoint(const char* ptr,
                                                       const char* end) {
  const std::size_t remaining = static_cast<std::size_t>(end - ptr);
  const auto [codepoint, length] = decode_utf8(ptr, remaining);
  return {codepoint, ptr + (length > 0 ? length : 1)};
}

}  // namespace unicode

#endif  // UNICODE_BASE_UTF8_DECODE_H_
