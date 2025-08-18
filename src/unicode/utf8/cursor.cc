// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/cursor.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>

#include "core/check.h"
#include "unicode/utf8/file_manager.h"

#if ENABLE_AVX2
#include <immintrin.h>
#endif

namespace unicode {

// returns 0 if valid or byte index of the invalid byte
std::size_t Utf8Cursor::init(Utf8FileManager* file_manager,
                             Utf8FileId file_id) {
  file_manager_ = file_manager;
  file_id_ = file_id;
  DCHECK(file_manager_);
  DCHECK_NE(file_id_, kInvalidFileId);

  cursor_state_.position = 0;
  cursor_state_.line = 1;
  cursor_state_.column = 1;
  status_ = Status::kInitialized;

  // validate the entire file content as valid utf-8
  const std::size_t result = validate_utf8();
  if (result == 0) [[likely]] {
    status_ = Status::kValid;
  } else {
    status_ = Status::kInvalid;
  }

  return result;
}

char32_t Utf8Cursor::peek_at(std::size_t offset) const {
  DCHECK_EQ(status_, Status::kValid);

  const std::size_t target_pos = cursor_state_.position + offset;
  const std::u8string_view content = file().content_u8();

  // DCHECK_LT(target_pos, content.size())
  //     << "peek target position is out of range";
  if (target_pos >= content.size()) [[unlikely]] {
    return '\0';
  }

  const char8_t* const ptr = content.data() + target_pos;
  const char8_t* const end = content.data() + content.size();

  const auto [codepoint, _] = decoder_.next_codepoint(ptr, end);
  return codepoint;
}

char32_t Utf8Cursor::next() {
  DCHECK_EQ(status_, Status::kValid);
  DCHECK(!eof());

  const char32_t codepoint = peek();
  const std::size_t bytes = peek_cache_.byte_count;
  peek_cache_.valid = false;

  cursor_state_.position += bytes;
  if (codepoint == '\n') [[unlikely]] {
    ++cursor_state_.line;
    cursor_state_.column = 1;
  } else {
    ++cursor_state_.column;
  }

  return peek();
}

bool Utf8Cursor::consume(char32_t code) {
  DCHECK_EQ(status_, Status::kValid);
  DCHECK(!eof());

  if (peek() == code) [[likely]] {
    next();
    return true;
  }

  return false;
}

// returns 0 if valid or byte index of the invalid byte
std::size_t Utf8Cursor::validate_utf8() const {
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

std::size_t Utf8Cursor::validate_utf8_avx2() const {
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

std::size_t Utf8Cursor::validate_utf8_avx2() const {
  return validate_utf8_scalar(0);
}

#endif

std::size_t Utf8Cursor::validate_utf8_scalar(std::size_t start_pos) const {
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

    if ((bytes - 1) >= remaining ||
        codepoint == Utf8Decoder::kInvalidUnicodePoint) {
      return static_cast<std::size_t>(ptr - begin);
    }
    ptr += bytes;
  }

  return 0;
}

}  // namespace unicode
