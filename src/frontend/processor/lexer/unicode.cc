// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/lexer/lexer.h"
#include "unicode/base/unicode_util.h"

namespace lexer {

Lexer::Result<base::Token> Lexer::unicode_token(char32_t current_codepoint,
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

  // note that `is_unicode_newline` must be evaluated before
  // `is_unicode_whitespace`, since it will return true even for newlines
  if (unicode::is_unicode_newline(current_codepoint)) {
    stream_.next();
    return create_token(TokenKind::kNewline, start, line, col);
  } else if (unicode::is_unicode_whitespace(current_codepoint)) {
    DCHECK(should_include_whitespace());
    while (!stream_.eof() && !unicode::is_unicode_newline(stream_.peek()) &&
           unicode::is_unicode_whitespace(stream_.peek())) {
      stream_.next();
    }
    return create_token(TokenKind::kWhitespace, start, line, col);
  }

  return err<Token>(Error::create(
      start, line, col, diagnostic::DiagnosticId::kUnrecognizedCharacter));
}

}  // namespace lexer
