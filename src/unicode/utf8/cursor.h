// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_UTF8_CURSOR_H_
#define UNICODE_UTF8_CURSOR_H_

#include <cstddef>

#include "unicode/base/unicode_export.h"
#include "unicode/utf8/decoder.h"
#include "unicode/utf8/file.h"

namespace unicode {

class UNICODE_EXPORT Utf8Cursor {
 public:
  enum class Status : uint8_t {
    kNotInitialized = 0,
    kInitialized = 1,
    kValid = 2,

    kInvalid = 3,
  };

  explicit Utf8Cursor() = default;
  ~Utf8Cursor() = default;

  Utf8Cursor(const Utf8Cursor&) = delete;
  Utf8Cursor& operator=(const Utf8Cursor&) = delete;

  Utf8Cursor(Utf8Cursor&&) = default;
  Utf8Cursor& operator=(Utf8Cursor&&) = default;

  // returns 0 if valid or byte index of the invalid byte
  std::size_t init(const Utf8File& file);

  char32_t peek_at(std::size_t offset) const;
  char32_t next();
  bool consume(char32_t code);

  inline char32_t peek() const {
    DCHECK_EQ(status_, Status::kValid);
    if (peek_cache_.valid) [[likely]] {
      return peek_cache_.codepoint;
    }

    if (eof()) [[unlikely]] {
      return 0;
    } else [[likely]] {
      const std::u8string_view content = file_->content_u8();
      const char8_t* const ptr = content.data() + cursor_state_.position;
      const char8_t* const end = content.data() + content.size();

      const auto [codepoint, next_ptr] = decoder_.next_codepoint(ptr, end);
      peek_cache_ = {codepoint, static_cast<std::uint8_t>(next_ptr - ptr),
                     true};
      return codepoint;
    }
  }

  inline std::size_t position() const { return cursor_state_.position; }

  inline std::size_t line() const { return cursor_state_.line; }

  inline std::size_t column() const { return cursor_state_.column; }

  inline bool eof() const {
    return cursor_state_.position >= file_->content().size();
  }

  inline const Utf8File& file() const { return *file_; }

  inline Status status() const { return status_; }

 private:
  // returns 0 if valid or byte index of the invalid byte
  std::size_t validate_utf8() const;

  // use avx2 simd if available
  std::size_t validate_utf8_avx2() const;

  // start_pos is an offset (in bytes) from beginning of file to start
  // validating.
  std::size_t validate_utf8_scalar(std::size_t start_pos = 0) const;

  struct CursorState {
    std::size_t position = 0;
    std::size_t line = 1;
    std::size_t column = 1;
  };

  struct PeekCache {
    char32_t codepoint = 0;
    uint8_t byte_count = 0;
    bool valid : 1 = false;
  };

  mutable PeekCache peek_cache_;
  CursorState cursor_state_;
  const Utf8File* file_ = nullptr;
  Utf8Decoder decoder_;
  Status status_ = Status::kNotInitialized;
};

}  // namespace unicode

#endif  // UNICODE_UTF8_CURSOR_H_
