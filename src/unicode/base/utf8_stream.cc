// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/base/utf8_stream.h"

#include <cstddef>
#include <string>
#include <utility>

#include "unicode/base/unicode_util.h"
#include "unicode/base/utf8_decode.h"

namespace unicode {

std::size_t Utf8Stream::init(const core::File& file) {
  file_ = &file;
  data_ = source().data();
  size_ = source().size();

  cursor_.pos = 0;
  cursor_.line = 1;
  cursor_.col = 1;
  cached_peek_pos_ = static_cast<std::size_t>(-1);
  cached_peek_cp_ = 0;
  cached_peek_len_ = 0;

  rewind_index_ = 0;
  rewind_size_ = 0;

  std::size_t validation_result = validate();
  if (status_ == Utf8StreamStatus::kValid) {
    return 0;
  }
  return validation_result;
}

inline std::pair<uint32_t, std::size_t> Utf8Stream::decode(
    std::size_t pos) const {
  return decode_utf8(data_ + pos, size_ - pos);
}

inline void Utf8Stream::update_cursor_newline(uint32_t cp) {
  if (cp == '\n') [[unlikely]] {
    ++cursor_.line;
    cursor_.col = 1;
  } else [[likely]] {
    ++cursor_.col;
  }
}

inline void Utf8Stream::save_rewind_state() {
  const std::size_t new_index =
      (rewind_index_ + rewind_size_) & (kRewindHistorySize - 1);
  rewind_buffer_[new_index] = {cursor_.pos, cursor_.line, cursor_.col};

  if (rewind_size_ < kRewindHistorySize) {
    ++rewind_size_;
  } else {
    rewind_index_ = (rewind_index_ + 1) & (kRewindHistorySize - 1);
  }
}

uint32_t Utf8Stream::peek(std::size_t offset) const {
  DCHECK(data_);
  DCHECK_EQ(status_, Utf8StreamStatus::kValid);

  // fast path: offset 0 with valid cache
  if (offset == 0) [[likely]] {
    if (cursor_.pos == cached_peek_pos_) [[likely]] {
      return cached_peek_cp_;
    }

    // cache miss - decode and cache
    auto [cp, len] = decode(cursor_.pos);

    cached_peek_pos_ = cursor_.pos;
    cached_peek_cp_ = cp;
    cached_peek_len_ = static_cast<uint8_t>(len);
    return cp;
  }

  // offset > 0 path
  std::size_t pos = cursor_.pos;

  for (std::size_t i = 0; i < offset && pos < size_; ++i) {
    const auto [cp, len] = decode(pos);
    if (cp == 0) [[unlikely]] {
      return 0;
    }
    pos += len;
  }

  if (pos >= size_) [[unlikely]] {
    return 0;
  }

  auto [cp, _] = decode(pos);
  return cp;
}

std::size_t Utf8Stream::advance() {
  DCHECK(data_);
  DCHECK_EQ(status_, Utf8StreamStatus::kValid);
  DCHECK(!eof());

  uint32_t cp;
  std::size_t len;

  // use cached values if available
  if (cached_peek_pos_ == cursor_.pos && cached_peek_len_ > 0) {
    cp = cached_peek_cp_;
    len = cached_peek_len_;
  } else {
    const auto [decoded_cp, byte_len] = decode(cursor_.pos);
    cp = decoded_cp;
    len = byte_len;
  }

  if (len == 0) [[unlikely]] {
    return 0;
  }

  // save current state for rewind
  save_rewind_state();

  // update position
  cursor_.pos += len;
  update_cursor_newline(cp);

  // invalidate cache
  cached_peek_pos_ = static_cast<std::size_t>(-1);

  return len;
}

void Utf8Stream::rewind() {
  DCHECK_EQ(status_, Utf8StreamStatus::kValid);

  if (rewind_size_ == 0) [[unlikely]] {
    return;
  }

  rewind_index_ = (rewind_index_ + kRewindHistorySize - 1) % kRewindHistorySize;
  const auto& saved = rewind_buffer_[rewind_index_];

  cursor_.pos = saved.pos;
  cursor_.line = saved.line;
  cursor_.col = saved.col;
  --rewind_size_;

  // invalidate cache
  cached_peek_pos_ = static_cast<std::size_t>(-1);
}

#if ENABLE_AVX2
std::size_t Utf8Stream::validate_avx2() {
  std::size_t pos = 0;

  const std::size_t simd_end = size_ & ~63;  // 64 bytes at a time

  const __m256i ascii_mask = _mm256_set1_epi8(0x7f);

  while (pos < simd_end) {
    // load and check two 32-byte chunks
    __m256i chunk1 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data_ + pos));
    __m256i chunk2 =
        _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data_ + pos + 32));

    __m256i cmp1 = _mm256_cmpgt_epi8(chunk1, ascii_mask);
    __m256i cmp2 = _mm256_cmpgt_epi8(chunk2, ascii_mask);

    // combine results
    __m256i combined = _mm256_or_si256(cmp1, cmp2);

    if (_mm256_movemask_epi8(combined) == 0) {
      pos += 64;
      continue;
    }

    // found non-ascii, fall back to scalar validation
    return validate_scalar(pos);
  }

  // validate remaining bytes
  return validate_scalar(pos);
}
#endif

std::size_t Utf8Stream::validate_scalar(std::size_t start_pos) {
  std::size_t pos = start_pos;

  while (pos < size_) {
    const auto [cp, len] = decode(pos);
    if (len == 0 || cp == kInvalidUnicodePoint) [[unlikely]] {
      status_ = Utf8StreamStatus::kInvalid;
      return pos;
    }
    pos += len;
  }

  status_ = Utf8StreamStatus::kValid;
  return 0;
}

std::size_t Utf8Stream::validate() {
  DCHECK(file_);
  DCHECK_NE(status_, Utf8StreamStatus::kValid);

#if ENABLE_AVX2
  return validate_avx2();
#else
  return validate_scalar();
#endif
}

}  // namespace unicode
