// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "core/base/string_util.h"
#include "frontend/processor/lexer/lexer.h"
#include "unicode/base/unicode_util.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_char() {
  const std::size_t start = cursor_.position();
  const std::size_t line = cursor_.line();
  const std::size_t col = cursor_.column();

  // consume the opening '\''
  cursor_.next();

  if (cursor_.eof()) {
    return err<Token>(Error::create(
        start, line, col, diagnostic::DiagId::kUnterminatedCharacterLiteral));
  }

  if (cursor_.peek() == '\\') {
    // consume '\'
    cursor_.next();

    if (cursor_.eof()) {
      return err<Token>(Error::create(
          start, line, col, diagnostic::DiagId::kUnterminatedCharacterLiteral));
    }

    const char32_t esc = cursor_.peek();
    if (unicode::is_ascii(esc) && core::is_valid_escape_sequence('\\', esc)) {
      // consume escape
      cursor_.next();
    } else if (esc == 'x') {
      // hex escape
      // consume 'x'
      cursor_.next();
      std::string buf;
      while (core::is_ascii_hex_digit(cursor_.peek())) {
        buf += static_cast<char>(cursor_.peek());
        cursor_.next();
      }
      if (!core::is_valid_hex_escape("x" + buf)) {
        return err<Token>(Error::create(
            start, line, col, diagnostic::DiagId::kInvalidEscapeSequence));
      }
    } else if (esc == 'u' || esc == 'U') {
      // unicode escape
      const uint8_t expected_digits = (esc == 'u' ? 4 : 8);
      std::string buf;
      buf += static_cast<char>(esc);
      // consume 'u'
      cursor_.next();

      for (int i = 0; i < expected_digits; ++i) {
        if (!core::is_ascii_hex_digit(cursor_.peek())) {
          break;
        }
        buf += static_cast<char>(cursor_.peek());
        cursor_.next();
      }

      if (buf.size() != (1 + expected_digits) ||
          !core::is_valid_unicode_escape(buf)) {
        return err<Token>(Error::create(
            start, line, col, diagnostic::DiagId::kInvalidUtfSequence));
      }

    } else if (core::is_ascii_octal_digit(esc)) {
      // octal escape (up to 3 digits)
      std::string buf;
      for (int i = 0; i < 3 && (core::is_ascii_octal_digit(cursor_.peek()));
           ++i) {
        buf += static_cast<char>(cursor_.peek());
        cursor_.next();
      }
      if (!core::is_valid_octal_escape(buf)) {
        return err<Token>(Error::create(
            start, line, col, diagnostic::DiagId::kInvalidEscapeSequence));
      }

    } else {
      // unknown escape
      return err<Token>(Error::create(
          start, line, col, diagnostic::DiagId::kInvalidEscapeSequence));
    }

  } else {
    // consume literal character
    cursor_.next();
  }

  if (cursor_.eof() || cursor_.peek() != '\'') {
    return err<Token>(Error::create(
        start, line, col, diagnostic::DiagId::kUnterminatedCharacterLiteral));
  }

  // consume the closing '\''
  cursor_.next();

  return create_token(TokenKind::kLiteralChar, start, line, col);
}

}  // namespace lexer
