// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"
#include "unicode/base/unicode_util.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::ascii_token(char current_char,
                                               std::size_t start,
                                               std::size_t line,
                                               std::size_t col) {
  if (unicode::is_xid_start(current_char)) {
    return identifier_or_keyword();
  }

  if (unicode::is_decimal_number(current_char)) {
    return literal_numeric();
  }

  // string literals
  if (current_char == '"') {
    return literal_str();
  }

  // character literals
  if (current_char == '\'') {
    return literal_char();
  }

  if (current_char == '\\' && !cursor_.eof()) {
    char32_t next_codepoint = cursor_.peek_at(1);
    if (unicode::is_ascii(next_codepoint)) {
      char next_char = static_cast<char>(next_codepoint);
      if (!core::is_valid_escape_sequence(current_char, next_char)) {
        return err<Token>(Error::create(
            start, line, col, diagnostic::DiagId::kInvalidEscapeSequence));
      }
    }
  }

  // consume the current character
  cursor_.next();

  char32_t next_codepoint = cursor_.peek();
  char next_char = unicode::is_ascii(next_codepoint)
                       ? static_cast<char>(next_codepoint)
                       : '\0';

  return other_token(current_char, next_char, start, line, col);
}

}  // namespace lexer
