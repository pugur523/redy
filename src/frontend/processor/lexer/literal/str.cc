// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "core/base/string_util.h"
#include "frontend/processor/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_str() {
  const std::size_t start = cursor_.position();
  const std::size_t line = cursor_.line();
  const std::size_t col = cursor_.column();

  cursor_.next();  // consume the opening '"'

  // consume characters until closing '"' or eof
  while (!cursor_.eof()) {
    const char current = cursor_.peek();
    if (current == '"') {
      break;
    }

    if (current == '\\') {
      // consume '\'
      cursor_.next();

      if (cursor_.eof()) {
        break;
      }

      const char esc = cursor_.peek();
      if (core::is_ascii_char(esc) &&
          core::is_valid_escape_sequence('\\', esc)) {
        cursor_.next();
      } else if (esc == 'x') {
        // \xHH: must be followed by at least 1 hex digit
        std::string buf;
        buf += esc;
        // consume 'x'
        cursor_.next();
        while (core::is_ascii_hex_digit(cursor_.peek())) {
          buf += static_cast<char>(cursor_.peek());
          cursor_.next();
        }
        if (!core::is_valid_hex_escape(buf)) {
          return err<Token>(
              Error::create(start, line, col,
                            diagnostic::DiagnosticId::kInvalidEscapeSequence));
        }
      } else if (esc == 'u' || esc == 'U') {
        std::string buf;
        buf += esc;
        // consume 'u' or 'U'
        cursor_.next();
        const uint8_t count = (esc == 'u' ? 4 : 8);
        for (uint8_t i = 0;
             i < count && core::is_ascii_hex_digit(cursor_.peek()); ++i) {
          buf += static_cast<char>(cursor_.peek());
          cursor_.next();
        }
        if (!core::is_valid_unicode_escape(buf)) {
          return err<Token>(
              Error::create(start, line, col,
                            diagnostic::DiagnosticId::kInvalidEscapeSequence));
        }
      } else if (core::is_ascii_octal_digit(cursor_.peek())) {
        std::string buf;
        const char c = cursor_.peek();
        for (uint8_t i = 0; i < 3 && c >= '0' && c <= '7'; ++i) {
          buf += c;
          cursor_.next();
        }
        if (!core::is_valid_octal_escape(buf)) {
          return err<Token>(
              Error::create(start, line, col,
                            diagnostic::DiagnosticId::kInvalidEscapeSequence));
        }
      } else {
        return err<Token>(
            Error::create(start, line, col,
                          diagnostic::DiagnosticId::kInvalidEscapeSequence));
      }
    } else {
      cursor_.next();
    }
  }

  if (cursor_.eof()) {
    return err<Token>(
        Error::create(start, line, col,
                      diagnostic::DiagnosticId::kUnterminatedStringLiteral));
  }

  // consume the closing '"'
  cursor_.next();

  return create_token(TokenKind::kString, start, line, col);
}

}  // namespace lexer
