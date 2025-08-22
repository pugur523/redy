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

// static
std::pair<char32_t, std::size_t> Utf8Decoder::decode(const char8_t* input,
                                                     std::size_t size) {
  DCHECK_GT(size, 0);

  const auto* const bytes = reinterpret_cast<const unsigned char*>(input);
  const uint8_t b0 = bytes[0];

  const uint8_t len = utf8_sequence_length(b0);

  if (len == 0 || size < len) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  }
  if (len == 1) [[likely]] {
    // ascii fast path
    return {b0, 1};
  }

  // if (!is_valid_continuation_sequence(bytes, len)) [[unlikely]] {
  //   return {kInvalidUnicodePoint, 1};
  // }

  char32_t cp;
  switch (len) {
    case 2: {
      cp = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
      break;
    }
    case 3: {
      cp = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) |
           (bytes[2] & 0x3F);
      break;
    }
    case 4: {
      cp = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
           ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
      break;
    }
    default: {
      return {kInvalidUnicodePoint, 1};
    }
  }

  if (!is_valid_codepoint(cp, len)) [[unlikely]] {
    return {kInvalidUnicodePoint, 1};
  }

  return {cp, len};
}

// static
std::size_t Utf8Decoder::decode_bulk(const char8_t* input,
                                     std::size_t input_size,
                                     char32_t* output,
                                     std::size_t output_capacity) {
  std::size_t input_pos = 0;
  std::size_t output_pos = 0;

  // process ascii bulk first
  const std::size_t ascii_processed =
      process_ascii_bulk(input, input_size, output);
  input_pos += ascii_processed;
  output_pos += ascii_processed;

  // process remaining non-ascii
  while (input_pos < input_size && output_pos < output_capacity) {
    const auto [codepoint, consumed] =
        decode(input + input_pos, input_size - input_pos);
    output[output_pos++] = codepoint;
    input_pos += consumed;
  }

  return output_pos;
}

// static
std::size_t Utf8Decoder::process_ascii_bulk(const char8_t* input,
                                            std::size_t size,
                                            char32_t* output) {
  const auto* bytes = reinterpret_cast<const unsigned char*>(input);
  std::size_t processed = 0;

#if ENABLE_AVX2
  // process 32 bytes at a time
  while (processed + 32 <= size) {
    const __m256i chunk =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(bytes + processed));

    // check if all bytes are ascii (< 0x80)
    const __m256i ascii_mask = _mm256_cmpgt_epi8(_mm256_set1_epi8(0x80), chunk);
    if (_mm256_movemask_epi8(ascii_mask) != -1) {
      break;  // non-ascii found, exit bulk processing
    }

    // convert to 32-bit codepoints (zero-extend)
    const __m128i low = _mm256_extracti128_si256(chunk, 0);
    const __m128i high = _mm256_extracti128_si256(chunk, 1);

    // process in groups of 8
    for (int i = 0; i < 2; ++i) {
      __m128i part = (i == 0) ? low : high;
      __m256i wide = _mm256_cvtepu8_epi32(part);
      _mm256_storeu_si256(
          reinterpret_cast<__m256i*>(output + processed + i * 8), wide);

      part = _mm_unpackhi_epi64(part, part);
      wide = _mm256_cvtepu8_epi32(part);
      _mm256_storeu_si256(
          reinterpret_cast<__m256i*>(output + processed + i * 8 + 8), wide);
    }

    processed += 32;
  }
#endif

  // process remaining
  while (processed < size && bytes[processed] < 0x80) {
    output[processed] = bytes[processed];
    ++processed;
  }

  return processed;
}

}  // namespace unicode
