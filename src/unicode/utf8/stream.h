// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_UTF8_STREAM_H_
#define UNICODE_UTF8_STREAM_H_

#include <cstddef>
#include <memory>
#include <vector>

#include "unicode/base/unicode_export.h"
#include "unicode/utf8/decoder.h"
#include "unicode/utf8/file.h"
#include "unicode/utf8/file_manager.h"

namespace unicode {

class UNICODE_EXPORT Utf8Stream {
 public:
  enum class Status : uint8_t {
    kNotInitialized = 0,
    kInitialized = 1,
    kValid = 2,
    kInvalid = 3,
  };

  explicit Utf8Stream() = default;
  ~Utf8Stream() = default;

  Utf8Stream(const Utf8Stream&) = delete;
  Utf8Stream& operator=(const Utf8Stream&) = delete;

  Utf8Stream(Utf8Stream&&) noexcept = default;
  Utf8Stream& operator=(Utf8Stream&&) noexcept = default;

  // validates and decodes entire content
  std::size_t init(Utf8FileManager* file_manager, Utf8FileId file_id);

  inline char32_t peek() const {
    DCHECK_EQ(status_, Status::kValid);
    if (eof()) [[unlikely]] {
      return 0;
    }
    return codepoints_[position_];
  }

  inline char32_t peek_at(std::size_t offset) const {
    DCHECK_EQ(status_, Status::kValid);
    if (eof_at(offset)) [[unlikely]] {
      return 0;
    }
    const std::size_t target_pos = position_ + offset;
    return codepoints_[target_pos];
  }

  inline char32_t next() {
    DCHECK_EQ(status_, Status::kValid);
    if (eof()) [[unlikely]] {
      return 0;
    }

    const char32_t old = codepoints_[position_];

    // update position tracking
    ++position_;
    if (old == '\n') [[unlikely]] {
      ++line_;
      column_ = 1;
    } else {
      ++column_;
    }

    return peek();
  }

  inline bool consume(char32_t expected) {
    DCHECK_EQ(status_, Status::kValid);
    if (peek() == expected) [[likely]] {
      next();
      return true;
    }
    return false;
  }

  // bulk consumption for pattern matching
  inline bool consume_sequence(const char32_t* sequence, std::size_t length) {
    DCHECK_EQ(status_, Status::kValid);
    if (position_ + length > codepoints_.size()) {
      return false;
    }

    for (std::size_t i = 0; i < length; ++i) {
      if (codepoints_[position_ + i] != sequence[i]) {
        return false;
      }
    }

    // consume all characters
    for (std::size_t i = 0; i < length; ++i) {
      next();
    }
    return true;
  }

  // position management
  inline std::size_t position() const { return position_; }
  inline std::size_t line() const { return line_; }
  inline std::size_t column() const { return column_; }
  inline bool eof() const { return position_ >= codepoints_.size(); }
  inline bool eof_at(std::size_t n) const {
    return position_ + n >= codepoints_.size();
  }

  struct Position {
    std::size_t pos;
    std::size_t line;
    std::size_t column;
  };

  inline void reset() { restore_position({}); }

  inline const std::vector<char32_t>& codepoints() const { return codepoints_; }
  inline Status status() const { return status_; }

  inline const Utf8File& file() const {
    DCHECK(file_manager_);
    DCHECK_NE(file_id_, kInvalidFileId);
    return file_manager_->loaded_file(file_id_);
  }

 private:
  inline Position save_position() const { return {position_, line_, column_}; }

  inline void restore_position(const Position& pos) {
    position_ = pos.pos;
    line_ = pos.line;
    column_ = pos.column;
  }

  // pre-decode entire utf-8 content into codepoint buffer
  std::size_t decode_content();

  // returns 0 if valid or byte index of the invalid byte
  std::size_t validate_utf8() const;

  // use avx2 simd if available
  std::size_t validate_utf8_avx2() const;

  // start_pos is an offset (in bytes) from beginning of file to start
  // validating.
  std::size_t validate_utf8_scalar(std::size_t start_pos = 0) const;

  std::vector<char32_t> codepoints_;  // pre-decoded codepoints
  std::size_t position_ = 0;
  std::size_t line_ = 1;
  std::size_t column_ = 1;

  Utf8FileManager* file_manager_ = nullptr;
  Utf8FileId file_id_ = kInvalidFileId;
  Status status_ = Status::kNotInitialized;

  Utf8Decoder decoder_;
};

}  // namespace unicode

#endif  // UNICODE_UTF8_STREAM_H_
