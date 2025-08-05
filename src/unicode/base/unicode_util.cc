// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/base/unicode_util.h"

namespace unicode::detail {

#if ENABLE_AVX2

inline bool is_ascii_letters_bulk_avx2(const uint32_t* codepoints,
                                       bool* results,
                                       std::size_t count) {
  // process 8 element unit
  const std::size_t simd_count = count & ~7;

  // const __m256i zero = _mm256_setzero_si256();
  const __m256i ascii_max = _mm256_set1_epi32(0x7F);
  const __m256i upper_min = _mm256_set1_epi32('A');
  const __m256i upper_max = _mm256_set1_epi32('Z');
  const __m256i lower_min = _mm256_set1_epi32('a');
  const __m256i lower_max = _mm256_set1_epi32('z');

  for (std::size_t i = 0; i < simd_count; i += 8) {
    __m256i chars =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&codepoints[i]));

    // ascii range check
    __m256i is_ascii = _mm256_cmpgt_epi32(
        _mm256_add_epi32(ascii_max, _mm256_set1_epi32(1)), chars);

    // upper case check
    __m256i is_upper = _mm256_and_si256(
        _mm256_cmpgt_epi32(_mm256_add_epi32(chars, _mm256_set1_epi32(1)),
                           upper_min),
        _mm256_cmpgt_epi32(_mm256_add_epi32(upper_max, _mm256_set1_epi32(1)),
                           chars));

    // small case check
    __m256i is_lower = _mm256_and_si256(
        _mm256_cmpgt_epi32(_mm256_add_epi32(chars, _mm256_set1_epi32(1)),
                           lower_min),
        _mm256_cmpgt_epi32(_mm256_add_epi32(lower_max, _mm256_set1_epi32(1)),
                           chars));

    // combine results
    __m256i is_letter =
        _mm256_and_si256(is_ascii, _mm256_or_si256(is_upper, is_lower));

    // store result
    alignas(32) int32_t temp_results[8];
    _mm256_store_si256(reinterpret_cast<__m256i*>(temp_results), is_letter);

    for (int j = 0; j < 8; ++j) {
      results[i + j] = temp_results[j] != 0;
    }
  }

  // process remaining with normal way
  for (std::size_t i = simd_count; i < count; ++i) {
    results[i] = is_ascii(codepoints[i]) && is_ascii_letter(codepoints[i]);
  }

  return true;
}

inline bool is_ascii_digits_bulk_avx2(const uint32_t* codepoints,
                                      bool* results,
                                      std::size_t count) {
  const std::size_t simd_count = count & ~7;

  const __m256i ascii_max = _mm256_set1_epi32(0x7F);
  const __m256i digit_min = _mm256_set1_epi32('0');
  const __m256i digit_max = _mm256_set1_epi32('9');

  for (std::size_t i = 0; i < simd_count; i += 8) {
    __m256i chars =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&codepoints[i]));

    // ascii range check
    __m256i is_ascii = _mm256_cmpgt_epi32(
        _mm256_add_epi32(ascii_max, _mm256_set1_epi32(1)), chars);

    // digit check
    __m256i is_digit = _mm256_and_si256(
        _mm256_cmpgt_epi32(_mm256_add_epi32(chars, _mm256_set1_epi32(1)),
                           digit_min),
        _mm256_cmpgt_epi32(_mm256_add_epi32(digit_max, _mm256_set1_epi32(1)),
                           chars));

    __m256i result = _mm256_and_si256(is_ascii, is_digit);

    alignas(32) int32_t temp_results[8];
    _mm256_store_si256(reinterpret_cast<__m256i*>(temp_results), result);

    for (int j = 0; j < 8; ++j) {
      results[i + j] = temp_results[j] != 0;
    }
  }

  for (std::size_t i = simd_count; i < count; ++i) {
    results[i] = is_ascii(codepoints[i]) && is_ascii_digit(codepoints[i]);
  }

  return true;
}
#endif  // ENABLE_AVX2

}  // namespace unicode::detail
