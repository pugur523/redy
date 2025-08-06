// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_BASE_UTF8_STREAM_H_
#define UNICODE_BASE_UTF8_STREAM_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

#include "core/base/file_util.h"
#include "core/check.h"
#include "unicode/base/unicode_export.h"

#if ENABLE_AVX2
#include <immintrin.h>
#endif

namespace unicode {

enum class Utf8StreamStatus : uint8_t {
  kUnknown = 0,
  kNotInitialized = 1,

  kValid = 2,

  kInvalid = 3,
};

class UNICODE_EXPORT Utf8Stream {
 public:
  Utf8Stream() = default;
  ~Utf8Stream() = default;

  Utf8Stream(const Utf8Stream&) = delete;
  Utf8Stream& operator=(const Utf8Stream&) = delete;

  Utf8Stream(Utf8Stream&&) = default;
  Utf8Stream& operator=(Utf8Stream&&) = default;

  // returns 0 if valid, or position of first invalid unicode character
  std::size_t init(const core::File& file);

  inline bool valid() const { return status_ == Utf8StreamStatus::kValid; }

  uint32_t peek(std::size_t offset = 0) const;

  // advances to next codepoint, returns how many bytes advanced
  std::size_t advance();

  // rewinds the last codepoint, do nothing if the cursor is at the top
  void rewind();

  inline bool eof() const {
    DCHECK(data_);
    return cursor_.pos >= file_->source().size();
  }

  inline std::size_t position() const { return cursor_.pos; }
  inline std::size_t line() const { return cursor_.line; }
  inline std::size_t column() const { return cursor_.col; }
  inline const core::File& file() const { return *file_; }
  inline const std::string& source() const { return file_->source(); }

 private:
#if ENABLE_AVX2
  std::size_t validate_avx2();
#endif
  std::size_t validate_scalar(std::size_t start_pos = 0);
  std::size_t validate();

  inline std::pair<uint32_t, std::size_t> decode(std::size_t pos) const;
  inline void update_cursor_newline(uint32_t cp);
  inline void save_rewind_state();

  static constexpr std::size_t kRewindHistorySize = 16;
  static_assert((kRewindHistorySize & (kRewindHistorySize - 1)) == 0,
                "kRewindHistorySize must be a power of 2");

  Utf8StreamStatus status_ = Utf8StreamStatus::kNotInitialized;

  const core::File* file_ = nullptr;
  const char* data_ = nullptr;
  std::size_t size_ = 0;

  struct {
    std::size_t pos = 0;
    std::size_t line = 1;
    std::size_t col = 1;
  } cursor_;

  // cache for peek(0) - most common case
  mutable std::size_t cached_peek_pos_ = static_cast<std::size_t>(-1);
  mutable uint32_t cached_peek_cp_ = 0;
  mutable uint8_t cached_peek_len_ = 0;

  struct RewindEntry {
    std::size_t pos;
    std::size_t line;
    std::size_t col;
  };

  std::array<RewindEntry, kRewindHistorySize> rewind_buffer_{};
  std::size_t rewind_index_ = 0;
  std::size_t rewind_size_ = 0;
};

}  // namespace unicode

#endif  // UNICODE_BASE_UTF8_STREAM_H_
