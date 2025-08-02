// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_LEXER_H_
#define FRONTEND_LEXER_LEXER_H_

#include <cctype>
#include <string>
#include <vector>

#include "core/base/string_util.h"
#include "frontend/base/data/char_stream.h"
#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/lexer/base/lex_error.h"
#include "frontend/lexer/base/lexer_export.h"

namespace lexer {

class LEXER_EXPORT Lexer {
 public:
  using Token = base::Token;
  using TokenKind = base::TokenKind;
  using CharStream = base::CharStream;

  template <typename T>
  using Result = diagnostic::Result<T, LexError>;
  template <typename T>
  using Results = diagnostic::Result<std::vector<T>, std::vector<LexError>>;

  explicit Lexer(core::FileManager* file_manager, core::FileId file_id);

  ~Lexer() = default;

  Result<Token> next_token();

  Results<Token> lex_all(bool strict = false);

 private:
  void skip_whitespace();

  Result<Token> identifier_or_keyword();
  Result<Token> literal_numeric();
  Result<Token> literal_str();
  Result<Token> literal_char();
  inline Result<Token> make_token(TokenKind kind,
                                  std::size_t start_pos,
                                  std::size_t line,
                                  std::size_t column) {
    const std::size_t end_pos = char_stream_.position();
    const core::FileId file_id = char_stream_.file_id();
    const std::size_t length = end_pos - start_pos;
    return Result(
        diagnostic::make_ok(Token(kind, file_id, line, column, length)));
  }

  inline static constexpr bool is_unicode_identifier_start(char c) {
    // FIXME: use unicode database
    // currently only support ascii alphabets and '_'
    return core::is_ascii_alphabet(c) || c == '_';
  }

  inline static constexpr bool is_unicode_identifier_char(char c) {
    // FIXME: use unicode database
    // currently support ascii alphabets, digits, '_'
    return core::is_ascii_alphabet(c) || core::is_ascii_digit(c) || c == '_';
  }

  CharStream char_stream_;

  static constexpr const std::size_t kPredictedTokensCount = 1024;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_LEXER_H_
