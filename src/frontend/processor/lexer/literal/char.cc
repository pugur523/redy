// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "core/base/string_util.h"
#include "frontend/processor/lexer/lexer.h"
#include "unicode/base/unicode_util.h"

namespace lexer {

Lexer::Result<base::Token> Lexer::literal_char() {
  const std::size_t start = stream_.position();
  const std::size_t line = stream_.line();
  const std::size_t col = stream_.column();

  // consume the opening '\''
  stream_.next();

  if (stream_.eof()) {
    return err<Token>(Error::create(
        start, line, col, diagnostic::DiagId::kUnterminatedCharacterLiteral));
  }

  if (stream_.peek() == '\\') {
    // consume '\'
    stream_.next();

    if (stream_.eof()) {
      return err<Token>(Error::create(
          start, line, col, diagnostic::DiagId::kUnterminatedCharacterLiteral));
    }

    const char32_t esc = stream_.peek();
    if (unicode::is_ascii(esc) && core::is_valid_escape_sequence('\\', esc)) {
      // consume escape
      stream_.next();
    } else if (esc == 'x') {
      // hex escape
      // consume 'x'
      stream_.next();
      std::string buf;
      while (core::is_ascii_hex_digit(stream_.peek())) {
        buf += static_cast<char>(stream_.peek());
        stream_.next();
      }
      if (!core::is_valid_hex_escape("x" + buf)) {
        return err<Token>(Error::create(start, line, col,
                                        diagnostic::DiagId::kInvalidHexEscape));
      }
    } else if (esc == 'u' || esc == 'U') {
      // unicode escape
      const uint8_t expected_digits = (esc == 'u' ? 4 : 8);
      std::string buf;
      buf += static_cast<char>(esc);
      // consume 'u'
      stream_.next();

      for (int i = 0; i < expected_digits; ++i) {
        if (!core::is_ascii_hex_digit(stream_.peek())) {
          break;
        }
        buf += static_cast<char>(stream_.peek());
        stream_.next();
      }

      if (buf.size() != (1 + expected_digits) ||
          !core::is_valid_unicode_escape(buf)) {
        return err<Token>(Error::create(
            start, line, col, diagnostic::DiagId::kInvalidUnicodeEscape));
      }

    } else if (core::is_ascii_octal_digit(esc)) {
      // octal escape (up to 3 digits)
      std::string buf;
      for (int i = 0; i < 3 && (core::is_ascii_octal_digit(stream_.peek()));
           ++i) {
        buf += static_cast<char>(stream_.peek());
        stream_.next();
      }
      if (!core::is_valid_octal_escape(buf)) {
        return err<Token>(Error::create(
            start, line, col, diagnostic::DiagId::kInvalidOctalEscape));
      }

    } else {
      // unknown escape
      return err<Token>(Error::create(
          start, line, col, diagnostic::DiagId::kInvalidCharacterEscape));
    }

  } else {
    // consume literal character
    stream_.next();
  }

  if (stream_.eof() || stream_.peek() != '\'') {
    return err<Token>(Error::create(
        start, line, col, diagnostic::DiagId::kUnterminatedCharacterLiteral));
  }

  // consume the closing '\''
  stream_.next();

  return create_token(TokenKind::kCharacter, start, line, col);
}

}  // namespace lexer
