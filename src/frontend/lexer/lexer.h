// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_LEXER_H_
#define FRONTEND_LEXER_LEXER_H_

#include <memory>
#include <string>
#include <vector>

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

  explicit Lexer(const core::File* file);

  ~Lexer() = default;

  Result<Token> next_token();

  Results<Token> lex_all(bool strict = false);

 private:
  void skip_whitespace();

  Result<Token> identifier_or_keyword();
  Result<Token> literal_numeric();
  Result<Token> literal_str();
  Result<Token> literal_char();

  Result<Token> ascii_token(char current_char,
                            std::size_t start,
                            std::size_t line,
                            std::size_t col);

  Result<Token> unicode_token(uint32_t current_codepoint,
                              std::size_t start,
                              std::size_t line,
                              std::size_t col);

  // operator or punctuation
  Result<Token> other_token(char current,
                            char next,
                            std::size_t start,
                            std::size_t line,
                            std::size_t col);

  inline Result<Token> make_token(TokenKind kind,
                                  std::size_t start_pos,
                                  std::size_t line,
                                  std::size_t column) {
    const std::size_t end_pos = char_stream_->position();
    const std::size_t length = end_pos - start_pos;
    return Result<Token>(
        diagnostic::make_ok(Token(kind, line, column, length)));
  }

  std::unique_ptr<CharStream> char_stream_ = nullptr;

  static constexpr const std::size_t kPredictedTokensCount = 1024;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_LEXER_H_
