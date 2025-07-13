// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_BASE_CHAR_STREAM_H_
#define FRONTEND_LEXER_BASE_CHAR_STREAM_H_

#include <cstddef>
#include <stack>
#include <string>

#include "core/base/file_manager.h"
#include "core/check.h"
#include "frontend/lexer/base/lexer_export.h"

namespace lexer {

class LEXER_EXPORT CharStream {
 public:
  using FileManager = core::FileManager;
  using FileId = core::FileId;
  using File = core::File;

  explicit CharStream(FileManager* file_manager, FileId file_id);
  ~CharStream() = default;

  CharStream(const CharStream&) = delete;
  CharStream& operator=(const CharStream&) = delete;

  CharStream(CharStream&&) noexcept = default;
  CharStream& operator=(CharStream&&) noexcept = default;

  char peek() const;
  char peek_ahead(std::size_t n) const;
  void advance();
  void rewind();

  inline constexpr bool eof() const { return pos_ >= file().source().size(); }

  inline constexpr std::size_t position() const { return pos_; }
  inline constexpr std::size_t line() const { return line_; }
  inline constexpr std::size_t column() const { return col_; }

  inline constexpr const FileManager* file_manager() const {
    DCHECK(file_manager_);
    return file_manager_;
  }
  inline constexpr FileId file_id() const { return file_id_; }

  inline constexpr const CharStream::File& file() const {
    DCHECK(file_manager_);
    return file_manager_->file(file_id_);
  }

 private:
  std::size_t pos_ = 0;
  std::size_t line_ = 1;
  std::size_t col_ = 1;
  FileManager* file_manager_;
  FileId file_id_;

  struct History {
    std::size_t pos = 0;
    std::size_t line = 0;
    std::size_t col = 0;
  };
  std::stack<History> history_;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_BASE_CHAR_STREAM_H_
