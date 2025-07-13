// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_TOKEN_TOKEN_H_
#define FRONTEND_LEXER_TOKEN_TOKEN_H_

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "core/base/file_util.h"
#include "core/base/source_location.h"
#include "core/base/string_util.h"
#include "core/check.h"
#include "frontend/lexer/base/lexer_export.h"
#include "frontend/lexer/token/token_kind.h"

namespace lexer {

class LEXER_EXPORT Token {
 public:
  Token(TokenKind kind,
        const core::FileManager* file_manager,
        core::SourceLocation&& location,
        std::size_t length);

  Token(TokenKind kind,
        const core::FileManager* file_manager,
        core::FileId file_id,
        std::size_t line,
        std::size_t column,
        std::size_t length);

  Token() = delete;

  ~Token() = default;

  Token(const Token&) = delete;
  Token& operator=(const Token&) = delete;

  Token(Token&&) noexcept = default;
  Token& operator=(Token&&) noexcept = default;

  inline TokenKind kind() const { return kind_; }

  inline const std::string_view lexeme() const {
    DCHECK(file_manager_);
    return std::string_view(
        file_manager_->file(location_.file_id()).line(location_.line()).data() +
            location_.column() - 1,
        length_);
  }

  inline const core::SourceLocation& location() const { return location_; }

  inline void dump(char* buf, std::size_t buf_size) const {
    char* cursor = buf;
    core::write_format(cursor, buf + buf_size, "{} ({})", to_string(kind_),
                       lexeme());
  }

  inline void dump_detailed(char* buf, std::size_t buf_size) {
    char* cursor = buf;
    core::write_format(
        cursor, buf + buf_size,
        "token:\n  kind: {} ({})\n  lexeme: {}\n  position: {}:{}",
        to_string(kind_), std::to_string(static_cast<int8_t>(kind_)), lexeme(),
        location_.line(), location_.column());
  }

  inline std::string dump() {
    char buf[512];
    dump(buf, 512);
    return std::string(buf);
  }

  inline std::string dump_detailed() {
    char buf[2048];
    dump_detailed(buf, 2048);
    return std::string(buf);
  }

 private:
  core::SourceLocation location_;
  std::size_t length_ = 0;
  const core::FileManager* file_manager_ = nullptr;
  TokenKind kind_ = TokenKind::kUnknown;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_TOKEN_TOKEN_H_
