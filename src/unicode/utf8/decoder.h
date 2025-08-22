// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_UTF8_DECODER_H_
#define UNICODE_UTF8_DECODER_H_

#include <cstddef>
#include <cstdint>
#include <utility>

#include "unicode/base/unicode_export.h"

namespace unicode {

class UNICODE_EXPORT Utf8Decoder {
 public:
  Utf8Decoder() = default;
  ~Utf8Decoder() = default;

  Utf8Decoder(const Utf8Decoder&) = delete;
  Utf8Decoder& operator=(const Utf8Decoder&) = delete;

  Utf8Decoder(Utf8Decoder&&) noexcept = default;
  Utf8Decoder& operator=(Utf8Decoder&&) noexcept = default;

  static constexpr const char32_t kInvalidUnicodePoint = 0xFFFD;

  // returns (codepoint, byte_count) or (0xFFFD, 1) if invalid.
  std::pair<char32_t, std::size_t> decode(const char8_t* input,
                                          std::size_t size) const;

  // consumes one codepoint from utf8 string, returns (codepoint, next_ptr)
  inline std::pair<char32_t, const char8_t*> next_codepoint(
      const char8_t* ptr,
      const char8_t* end) const {
    const std::size_t remaining = static_cast<std::size_t>(end - ptr);
    const auto [codepoint, length] = decode(ptr, remaining);
    return {codepoint, ptr + (length > 0 ? length : 1)};
  }
};

}  // namespace unicode

#endif  // UNICODE_UTF8_DECODER_H_
