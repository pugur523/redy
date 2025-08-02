// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_TOKEN_TOKEN_H_
#define FRONTEND_BASE_TOKEN_TOKEN_H_

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>

#include "core/base/file_util.h"
#include "core/base/source_location.h"
#include "core/base/string_util.h"
#include "core/check.h"
#include "frontend/base/base_export.h"
#include "frontend/base/token/token_kind.h"

namespace base {

class BASE_EXPORT Token {
 public:
  Token(TokenKind kind,
        const core::SourceLocation& location,
        std::size_t length);

  Token(TokenKind kind,
        core::FileId file_id,
        std::size_t line,
        std::size_t column,
        std::size_t length);

  Token() = delete;

  ~Token() = default;

  Token(const Token&) = delete;
  Token& operator=(const Token&) = delete;

  Token(Token&&) = default;
  Token& operator=(Token&&) = default;

  inline const core::SourceLocation& location() const { return location_; }
  inline std::size_t length() const { return length_; }
  inline TokenKind kind() const { return kind_; }

  inline const std::string_view lexeme(
      const core::FileManager* file_manager) const {
    DCHECK(file_manager);
    return std::string_view(
        file_manager->file(location_.file_id()).line(location_.line()).data() +
            location_.column() - 1,
        length_);
  }

  inline void dump(const core::FileManager* file_manager,
                   char* buf,
                   std::size_t buf_size) const {
    char* cursor = buf;
    core::write_format(cursor, buf + buf_size, "{} ({})",
                       token_kind_to_string(kind_), lexeme(file_manager));
  }

  inline void dump_detailed(const core::FileManager* file_manager,
                            char* buf,
                            std::size_t buf_size) {
    char* cursor = buf;
    core::write_format(
        cursor, buf + buf_size,
        "[token]\n"
        "kind_str = \"{}\"\nkind_id = {}\n"
        "lexeme = \"{}\"\nline = {}\ncolumn = {}\n",
        token_kind_to_string(kind_), std::to_string(static_cast<int8_t>(kind_)),
        lexeme(file_manager), location_.line(), location_.column());
  }

  inline std::string dump(const core::FileManager* file_manager) {
    char buf[512];
    dump(file_manager, buf, 512);
    return std::string(buf);
  }

  inline std::string dump_detailed(const core::FileManager* file_manager) {
    char buf[2048];
    dump_detailed(file_manager, buf, 2048);
    return std::string(buf);
  }

 private:
  core::SourceLocation location_;
  std::size_t length_ = 0;
  TokenKind kind_ = TokenKind::kUnknown;
};

}  // namespace base

#endif  // FRONTEND_BASE_TOKEN_TOKEN_H_
