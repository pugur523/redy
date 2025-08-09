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
#include "core/base/source_range.h"
#include "core/base/string_util.h"
#include "core/check.h"
#include "frontend/base/base_export.h"
#include "frontend/base/token/token_kind.h"
#include "unicode/utf8/file.h"

namespace base {

class BASE_EXPORT Token {
 public:
  constexpr Token(TokenKind kind,
                  const core::SourceLocation& location,
                  std::size_t length)
      : range_(location, length), kind_(kind) {}

  constexpr Token(TokenKind kind, const core::SourceRange& range)
      : range_(range), kind_(kind) {}

  constexpr Token(TokenKind kind,
                  std::size_t line,
                  std::size_t column,
                  std::size_t length)
      : Token(kind, core::SourceRange(line, column, length)) {}

  Token() = delete;

  ~Token() = default;

  Token(const Token&) = delete;
  Token& operator=(const Token&) = delete;

  Token(Token&&) = default;
  Token& operator=(Token&&) = default;

  inline const core::SourceRange& range() const { return range_; }
  inline const core::SourceLocation& start() const { return range_.start(); }
  inline const core::SourceLocation end() const { return range_.end(); }
  inline std::size_t length() const { return range_.length(); }
  inline TokenKind kind() const { return kind_; }

  inline const std::u8string_view lexeme_u8(
      const unicode::Utf8File& file) const {
    return std::u8string_view(file.line_u8(range_.start().line()).data() +
                                  range_.start().column() - 1,
                              range_.length());
  }

  inline const std::string_view lexeme(const unicode::Utf8File& file) const {
    std::u8string_view lexeme_view = lexeme_u8(file);
    return std::string_view(reinterpret_cast<const char*>(lexeme_view.data()),
                            lexeme_view.size());
  }

  inline void dump(const unicode::Utf8File& file,
                   char* buf,
                   std::size_t buf_size) const {
    char* cursor = buf;
    core::write_format(cursor, buf + buf_size, "{} ({})",
                       token_kind_to_string(kind_),
                       std::string_view(lexeme(file)));
  }

  inline void dump_detailed(const unicode::Utf8File& file,
                            char* buf,
                            std::size_t buf_size) {
    char* cursor = buf;
    core::write_format(cursor, buf + buf_size,
                       "[token]\n"
                       "kind_str = \"{}\"\nkind_id = {}\n"
                       "lexeme = \"{}\"\nline = {}\ncolumn = {}\n",
                       token_kind_to_string(kind_),
                       std::to_string(static_cast<int8_t>(kind_)), lexeme(file),
                       range_.start().line(), range_.start().column());
  }

  inline std::string dump(const unicode::Utf8File& file) {
    char buf[512];
    dump(file, buf, 512);
    return std::string(buf);
  }

  inline std::string dump_detailed(const unicode::Utf8File& file) {
    char buf[2048];
    dump_detailed(file, buf, 2048);
    return std::string(buf);
  }

 private:
  core::SourceRange range_;
  TokenKind kind_ = TokenKind::kUnknown;
};

}  // namespace base

#endif  // FRONTEND_BASE_TOKEN_TOKEN_H_
