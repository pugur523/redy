// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/stream.h"

#include <vector>

#if ENABLE_AVX2
#include <immintrin.h>
#endif

namespace unicode {

std::size_t Utf8Stream::init(Utf8FileManager* file_manager,
                             Utf8FileId file_id) {
  file_manager_ = file_manager;
  file_id_ = file_id;

  DCHECK(file_manager_);
  DCHECK_NE(file_id_, kInvalidFileId);

  position_ = 0;
  line_ = 1;
  column_ = 1;
  status_ = Status::kInitialized;

  const std::size_t invalid_pos = validate_utf8();
  if (invalid_pos != 0) {
    status_ = Status::kInvalid;
    return invalid_pos;
  }

  const std::size_t decode_error = decode_content();
  if (decode_error != 0) {
    status_ = Status::kInvalid;
    return decode_error;
  }

  status_ = Status::kValid;
  return 0;
}

std::size_t Utf8Stream::decode_content() {
  const std::u8string_view content = file().content_u8();
  if (content.empty()) {
    return 0;
  }

  codepoints_.clear();
  // worst case is all ascii
  codepoints_.reserve(content.size());

  const std::size_t input_size = content.size();
  const std::size_t max_output = input_size;

  std::vector<char32_t> temp_buffer(max_output);
  const std::size_t decoded_count = decoder_.decode_bulk(
      content.data(), input_size, temp_buffer.data(), max_output);

  if (decoded_count == 0 && !content.empty()) {
    return 1;
  }

  // copy to final buffer with exact size
  codepoints_.assign(temp_buffer.begin(), temp_buffer.begin() + decoded_count);
  codepoints_.shrink_to_fit();

  return 0;
}

// returns 0 if valid or byte index of the invalid byte
std::size_t Utf8Stream::validate_utf8() const {
  DCHECK(file_manager_);
  DCHECK_NE(file_id_, kInvalidFileId);
  DCHECK_EQ(status_, Status::kInitialized);

#if ENABLE_AVX2
  return validate_utf8_avx2();
#else
  return validate_utf8_scalar(0);
#endif
}

#if ENABLE_AVX2

std::size_t Utf8Stream::validate_utf8_avx2() const {
  const std::u8string_view content = file().content_u8();
  const std::size_t total_size = content.size();
  if (total_size == 0) {
    return 0;  // empty file is valid
  }

  const char8_t* const begin = content.data();
  const char8_t* ptr = begin;

  // 64-byte block processing (2x 256-bit)
  constexpr std::size_t kBlock = 64;
  const std::size_t simd_bytes = total_size & ~(kBlock - 1);
  const char8_t* simd_end = begin + simd_bytes;

  const __m256i ascii_mask = _mm256_set1_epi8(static_cast<char>(0x7F));

  while (ptr < simd_end) {
    const __m256i chunk1 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr));
    const __m256i chunk2 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + 32));

    // any byte > 0x7F?
    const __m256i cmp1 = _mm256_cmpgt_epi8(chunk1, ascii_mask);
    const __m256i cmp2 = _mm256_cmpgt_epi8(chunk2, ascii_mask);
    const __m256i combined = _mm256_or_si256(cmp1, cmp2);

    if (_mm256_testz_si256(combined, combined)) {
      ptr += kBlock;
      continue;  // all ascii
    }

    // hit non-ascii -> break to scalar check from here
    break;
  }

  // if we scanned all full simd blocks and there's no remainder
  if (ptr == begin + total_size) {
    return 0;  // all ascii
  }

  // scalar validate remainder (including the block with first non-ascii)
  return validate_utf8_scalar(static_cast<std::size_t>(ptr - begin));
}

#else

std::size_t Utf8Stream::validate_utf8_avx2() const {
  return validate_utf8_scalar(0);
}

#endif

std::size_t Utf8Stream::validate_utf8_scalar(std::size_t start_pos) const {
  const std::u8string_view content = file().content_u8();
  const std::size_t total_size = content.size();
  if (total_size == 0 || start_pos >= total_size) {
    return 0;  // empty or nothing to validate
  }

  const char8_t* const begin = content.data();
  const char8_t* const end = begin + content.size();
  const char8_t* ptr = begin + start_pos;

  while (ptr < end) {
    const std::size_t remaining = static_cast<std::size_t>(end - ptr);
    const auto [codepoint, bytes] = decoder_.decode(ptr, remaining);

    if (codepoint == Utf8Decoder::kInvalidUnicodePoint ||
        (bytes - 1) >= remaining) {
      return static_cast<std::size_t>(ptr - begin);
    }
    ptr += bytes;
  }

  return 0;
}

}  // namespace unicode
