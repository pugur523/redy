// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_LEXER_H_
#define FRONTEND_LEXER_LEXER_H_

#include <cctype>
#include <string>
#include <vector>

#include "frontend/lexer/base/char_stream.h"
#include "frontend/lexer/base/lexer_export.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

class LEXER_EXPORT Lexer {
 public:
  using FileManager = core::FileManager;
  using FileId = core::FileId;

  explicit Lexer(FileManager* file_manager, FileId file_id);

  ~Lexer() = default;

  Token next_token();

  std::vector<Token> lex_all();

 private:
  void skip_whitespace_and_comments();

  Token identifier_or_keyword();
  Token literal_number();
  Token literal_string();
  Token literal_char();
  Token make_token(TokenKind kind,
                   std::size_t start_pos,
                   std::size_t line,
                   std::size_t col);

  CharStream char_stream_;

  static constexpr const std::size_t kPredictedTokensCount = 1024;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_LEXER_H_
