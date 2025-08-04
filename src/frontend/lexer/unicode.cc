// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"
#include "unicode/base/unicode_util.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::unicode_token(uint32_t current_codepoint,
                                                 std::size_t start,
                                                 std::size_t line,
                                                 std::size_t col) {
  // identifiers and keywords
  if (unicode::is_xid_start(current_codepoint)) {
    return identifier_or_keyword();
  }

  // numeric literals
  if (unicode::is_decimal_number(current_codepoint)) {
    return literal_numeric();
  }

  return Result<Token>(diagnostic::make_err(LexError::make(
      diagnostic::DiagnosticId::kUnrecognizedCharacter, start, line, col,
      "non-ascii character in operator/punctuation context")));
}

}  // namespace lexer
