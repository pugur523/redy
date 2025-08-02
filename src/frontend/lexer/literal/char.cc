// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "frontend/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_char() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // consume the opening '\''
  char_stream_.advance();

  if (char_stream_.eof()) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedCharacterLiteral,
                       start, line, col, "unterminated character literal")));
  }

  if (char_stream_.peek() == '\\') {
    // consume '\'
    char_stream_.advance();

    if (char_stream_.eof()) {
      return Result<Token>(diagnostic::make_err(LexError::make(
          diagnostic::DiagnosticId::kUnterminatedCharacterLiteral, start, line,
          col, "unterminated character literal")));
    }

    char esc = char_stream_.peek();
    if (core::is_ascii_char(esc) && core::is_valid_escape_sequence('\\', esc)) {
      char_stream_.advance();
    } else if (esc == 'x') {
      std::string buf;
      char_stream_.advance();
      while (std::isxdigit(char_stream_.peek())) {
        buf += char_stream_.peek();
        char_stream_.advance();
      }
      if (!core::is_valid_hex_escape("x" + buf)) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                           start, line, col, "\\x invalid hex escape")));
      }
    } else if (esc == 'u' || esc == 'U') {
      std::string buf;
      buf += esc;
      char_stream_.advance();
      int count = (esc == 'u' ? 4 : 8);
      for (int i = 0; i < count && std::isxdigit(char_stream_.peek()); ++i) {
        buf += char_stream_.peek();
        char_stream_.advance();
      }
      if (!core::is_valid_unicode_escape(buf)) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                           start, line, col, "invalid unicode escape")));
      }
    } else if (esc >= '0' && esc <= '7') {
      std::string buf;
      for (int i = 0;
           i < 3 && char_stream_.peek() >= '0' && char_stream_.peek() <= '7';
           ++i) {
        buf += char_stream_.peek();
        char_stream_.advance();
      }
      if (!core::is_valid_octal_escape(buf)) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                           start, line, col, "invalid octal escape")));
      }
    } else {
      return Result<Token>(diagnostic::make_err(
          LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                         start, line, col, "unknown escape sequence")));
    }
  } else {
    char_stream_.advance();
  }

  if (char_stream_.eof() || char_stream_.peek() != '\'') {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedCharacterLiteral,
                       start, line, col, "unterminated character literal")));
  }

  // consume the closing '\''
  char_stream_.advance();

  return make_token(TokenKind::kLiteralChar, start, line, col);
}

}  // namespace lexer
