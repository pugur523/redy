// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_LEXER_H_
#define FRONTEND_LEXER_LEXER_H_

#include <cctype>
#include <string>
#include <vector>

#include "frontend/diagnostic/data/result.h"
#include "frontend/lexer/base/char_stream.h"
#include "frontend/lexer/base/lex_error.h"
#include "frontend/lexer/base/lexer_export.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

class LEXER_EXPORT Lexer {
 public:
  template <typename T>
  using Result = diagnostic::Result<T, LexError>;
  template <typename T>
  using Results = diagnostic::Result<std::vector<T>, std::vector<LexError>>;

  explicit Lexer(core::FileManager* file_manager, core::FileId file_id);

  ~Lexer() = default;

  Result<Token> next_token();

  Results<Token> lex_all(bool strict = false);

 private:
  void skip_whitespace_and_comments();

  Result<Token> identifier_or_keyword();
  Result<Token> literal_numeric();
  Result<Token> literal_string();
  Result<Token> literal_char();
  Result<Token> make_token(TokenKind kind,
                           std::size_t start_pos,
                           std::size_t line,
                           std::size_t col);

  CharStream char_stream_;

  static constexpr const std::size_t kPredictedTokensCount = 1024;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_LEXER_H_
