// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "core/base/string_util.h"
#include "frontend/processor/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_str() {
  const std::size_t start = stream_.position();
  const std::size_t line = stream_.line();
  const std::size_t col = stream_.column();

  stream_.next();  // consume the opening '"'

  // consume characters until closing '"' or eof
  while (!stream_.eof()) {
    const char current = stream_.peek();
    if (current == '"') {
      break;
    }

    if (current == '\\') {
      // consume '\'
      stream_.next();

      if (stream_.eof()) {
        break;
      }

      const char esc = stream_.peek();
      if (core::is_ascii_char(esc) &&
          core::is_valid_escape_sequence('\\', esc)) {
        stream_.next();
      } else if (esc == 'x') {
        // \xHH: must be followed by at least 1 hex digit
        std::string buf;
        buf += esc;
        // consume 'x'
        stream_.next();
        while (core::is_ascii_hex_digit(stream_.peek())) {
          buf += static_cast<char>(stream_.peek());
          stream_.next();
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
        stream_.next();
        const uint8_t count = (esc == 'u' ? 4 : 8);
        for (uint8_t i = 0;
             i < count && core::is_ascii_hex_digit(stream_.peek()); ++i) {
          buf += static_cast<char>(stream_.peek());
          stream_.next();
        }
        if (!core::is_valid_unicode_escape(buf)) {
          return err<Token>(
              Error::create(start, line, col,
                            diagnostic::DiagnosticId::kInvalidEscapeSequence));
        }
      } else if (core::is_ascii_octal_digit(stream_.peek())) {
        std::string buf;
        const char c = stream_.peek();
        for (uint8_t i = 0; i < 3 && '0' <= c && c <= '7'; ++i) {
          buf += c;
          stream_.next();
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
      stream_.next();
    }
  }

  if (stream_.eof()) {
    return err<Token>(
        Error::create(start, line, col,
                      diagnostic::DiagnosticId::kUnterminatedStringLiteral));
  }

  // consume the closing '"'
  stream_.next();

  return create_token(TokenKind::kString, start, line, col);
}

}  // namespace lexer
