// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "core/base/string_util.h"
#include "frontend/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_char() {
  const std::size_t start = stream_->position();
  const std::size_t line = stream_->line();
  const std::size_t col = stream_->column();

  // consume the opening '\''
  stream_->advance();

  if (stream_->eof()) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedCharacterLiteral,
                       start, line, col, "unterminated character literal")));
  }

  if (stream_->peek() == '\\') {
    // consume '\'
    stream_->advance();

    if (stream_->eof()) {
      return Result<Token>(diagnostic::make_err(LexError::make(
          diagnostic::DiagnosticId::kUnterminatedCharacterLiteral, start, line,
          col, "unterminated character literal")));
    }

    const char esc = stream_->peek();
    if (core::is_ascii_char(esc) && core::is_valid_escape_sequence('\\', esc)) {
      // consume escape
      stream_->advance();
    } else if (esc == 'x') {
      // hex escape
      // consume 'x'
      stream_->advance();
      std::string buf;
      while (core::is_ascii_hex_digit(stream_->peek())) {
        buf += static_cast<char>(stream_->peek());
        stream_->advance();
      }
      if (!core::is_valid_hex_escape("x" + buf)) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                           start, line, col, "\\x invalid hex escape")));
      }
    } else if (esc == 'u' || esc == 'U') {
      // unicode escape
      const uint8_t expected_digits = (esc == 'u' ? 4 : 8);
      std::string buf;
      buf += esc;
      // consume 'u'
      stream_->advance();

      for (int i = 0; i < expected_digits; ++i) {
        if (!core::is_ascii_hex_digit(stream_->peek())) {
          break;
        }
        buf += static_cast<char>(stream_->peek());
        stream_->advance();
      }

      if (buf.size() != (1 + expected_digits) ||
          !core::is_valid_unicode_escape(buf)) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                           start, line, col, "invalid unicode escape")));
      }

    } else if (core::is_ascii_octal_digit(esc)) {
      // octal escape (up to 3 digits)
      std::string buf;
      for (int i = 0; i < 3 && (core::is_ascii_octal_digit(stream_->peek()));
           ++i) {
        buf += static_cast<char>(stream_->peek());
        stream_->advance();
      }
      if (!core::is_valid_octal_escape(buf)) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                           start, line, col, "invalid octal escape")));
      }

    } else {
      // unknown escape
      return Result<Token>(diagnostic::make_err(
          LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                         start, line, col, "unknown escape sequence")));
    }

  } else {
    // consume literal character
    stream_->advance();
  }

  if (stream_->eof() || stream_->peek() != '\'') {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedCharacterLiteral,
                       start, line, col, "unterminated character literal")));
  }

  // consume the closing '\''
  stream_->advance();

  return make_token(TokenKind::kLiteralChar, start, line, col);
}

}  // namespace lexer
