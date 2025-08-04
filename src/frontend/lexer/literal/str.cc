// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "core/base/string_util.h"
#include "frontend/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_str() {
  const std::size_t start = char_stream_->position();
  const std::size_t line = char_stream_->line();
  const std::size_t col = char_stream_->column();

  char_stream_->advance();  // consume the opening '"'

  // consume characters until closing '"' or eof
  while (!char_stream_->eof()) {
    const char current = char_stream_->peek();
    if (current == '"') {
      break;
    }

    if (current == '\\') {
      // consume '\'
      char_stream_->advance();

      if (char_stream_->eof()) {
        break;
      }

      const char esc = char_stream_->peek();
      if (core::is_ascii_char(esc) &&
          core::is_valid_escape_sequence('\\', esc)) {
        char_stream_->advance();
      } else if (esc == 'x') {
        // \xHH: must be followed by at least 1 hex digit
        std::string buf;
        buf += esc;
        // consume 'x'
        char_stream_->advance();
        while (core::is_ascii_hex_digit(char_stream_->peek())) {
          buf += char_stream_->peek();
          char_stream_->advance();
        }
        if (!core::is_valid_hex_escape(buf)) {
          return Result<Token>(diagnostic::make_err(
              LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                             start, line, col, "\\x invalid hex escape")));
        }
      } else if (esc == 'u' || esc == 'U') {
        std::string buf;
        buf += esc;
        // consume 'u' or 'U'
        char_stream_->advance_codepoint();
        const uint8_t count = (esc == 'u' ? 4 : 8);
        for (uint8_t i = 0;
             i < count && core::is_ascii_hex_digit(char_stream_->peek()); ++i) {
          buf += char_stream_->peek();
          char_stream_->advance();
        }
        if (!core::is_valid_unicode_escape(buf)) {
          return Result<Token>(diagnostic::make_err(
              LexError::make(diagnostic::DiagnosticId::kInvalidEscapeSequence,
                             start, line, col, "invalid unicode escape")));
        }
      } else if (core::is_ascii_octal_digit(char_stream_->peek_codepoint())) {
        std::string buf;
        const char c = char_stream_->peek();
        for (uint8_t i = 0; i < 3 && c >= '0' && c <= '7'; ++i) {
          buf += c;
          char_stream_->advance();
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
      char_stream_->advance_codepoint();
    }
  }

  if (char_stream_->eof()) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedStringLiteral,
                       start, line, col, "unterminated string literal")));
  }

  // consume the closing '"'
  char_stream_->advance();

  return make_token(TokenKind::kLiteralStr, start, line, col);
}

}  // namespace lexer
