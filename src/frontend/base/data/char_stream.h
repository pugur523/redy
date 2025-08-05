// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_DATA_CHAR_STREAM_H_
#define FRONTEND_BASE_DATA_CHAR_STREAM_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <utility>

#include "core/base/file_manager.h"
#include "core/check.h"
#include "frontend/base/base_export.h"

namespace base {

class BASE_EXPORT CharStream {
 public:
  using FileManager = core::FileManager;
  using FileId = core::FileId;
  using File = core::File;

  explicit CharStream(const File* file);
  ~CharStream() = default;

  CharStream(const CharStream&) = delete;
  CharStream& operator=(const CharStream&) = delete;

  CharStream(CharStream&&) = default;
  CharStream& operator=(CharStream&&) = default;

  inline char peek(std::size_t n = 0) const {
    return (cursor_.pos + n < file().source().size())
               ? file().source()[cursor_.pos + n]
               : '\0';
  }

  void advance();
  void rewind();

  // unicode
  uint32_t peek_codepoint(std::size_t n = 0) const;
  void advance_codepoint();
  void advance_codepoint_fast();

  std::size_t codepoint_byte_length() const;
  bool is_valid_utf8_current_codepoint() const;
  bool is_valid_utf8_whole_stream() const;

  // position tracking
  inline constexpr bool eof() const {
    return cursor_.pos >= file().source().size();
  }

  inline constexpr std::size_t position() const { return cursor_.pos; }
  inline constexpr std::size_t line() const { return cursor_.line; }
  inline constexpr std::size_t column() const { return cursor_.col; }
  inline constexpr std::size_t codepoint_column() const {
    return cursor_.cp_col;
  }

  inline constexpr const File& file() const {
    DCHECK(file_);
    return *file_;
  }

 private:
  void update_position_for_codepoint(uint32_t codepoint,
                                     std::size_t byte_length);

  static inline constexpr std::size_t utf8_sequence_length(
      unsigned char first_byte) noexcept {
    if (first_byte < 0x80) {
      return 1;
    }
    if ((first_byte >> 5) == 0x06) {
      return 2;
    }
    if ((first_byte >> 4) == 0x0E) {
      return 3;
    }
    if ((first_byte >> 3) == 0x1E) {
      return 4;
    }
    return 1;
  }

  static inline constexpr std::pair<uint32_t, std::size_t> decode_utf8_at(
      const char* data,
      std::size_t pos,
      std::size_t max_size) noexcept;

  struct Cursor {
    std::size_t pos = 0;
    std::size_t line = 0;
    std::size_t col = 0;     // byte based (ascii)
    std::size_t cp_col = 0;  // codepoint based (unicode)
  };
  Cursor cursor_{.pos = 0, .line = 1, .col = 1, .cp_col = 1};

  static constexpr std::size_t kRewindHistorySize = 32;
  std::array<Cursor, kRewindHistorySize> rewind_buffer_{};
  std::size_t rewind_index_ = 0;
  std::size_t rewind_size_ = 0;

  mutable std::optional<uint32_t> current_codepoint_cache_ = std::nullopt;
  mutable std::size_t cached_pos_ = 0;

  const File* file_ = nullptr;

  static constexpr uint32_t kInvalidUtfChar = 0xFFFD;
};

}  // namespace base

#endif  // FRONTEND_BASE_DATA_CHAR_STREAM_H_
