// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <cstdint>
#include <string>

#include "core/base/string_util.h"
#include "frontend/lexer/lexer.h"

namespace lexer {

namespace {

struct NumericMeta {
  bool has_digit : 1 = false;
  bool seen_dot : 1 = false;
  bool seen_exponent : 1 = false;
  bool is_base_prefixed : 1 = false;
};

}  // namespace

Lexer::Result<Lexer::Token> Lexer::literal_numeric() {
  const std::size_t start = cursor_.position();
  const std::size_t line = cursor_.line();
  const std::size_t col = cursor_.column();

  NumericMeta meta;

  const char32_t ch0 = cursor_.peek();
  const char32_t ch1 = cursor_.peek_at(1);

  // base prefixes (0x, 0b, 0o)
  if (ch0 == '0') {
    if (ch1 == 'x' || ch1 == 'X') {
      // hex
      meta.is_base_prefixed = true;
      cursor_.next();  // consume '0'
      cursor_.next();  // consume 'x'

      // consume hex digits
      while (core::is_ascii_hex_digit(cursor_.peek())) {
        meta.has_digit = true;
        cursor_.next();
      }

    } else if (ch1 == 'b' || ch1 == 'B') {
      // binary
      meta.is_base_prefixed = true;
      cursor_.next();  // consume '0'
      cursor_.next();  // consume 'b'

      // consume binary digits
      while (true) {
        const char32_t c = cursor_.peek();
        if (core::is_ascii_binary_digit(c)) {
          meta.has_digit = true;
          cursor_.next();
        } else {
          break;
        }
      }

      // check for invalid binary digits (2-9)
      if (core::is_ascii_digit(cursor_.peek()) &&
          !core::is_ascii_binary_digit(cursor_.peek())) {
        return err<Token>(
            Error::create(start, line, col,
                          diagnostic::DiagnosticId::kInvalidNumericLiteral));
      }

    } else if (ch1 == 'o' || ch1 == 'O') {
      // octal
      meta.is_base_prefixed = true;
      cursor_.next();  // consume '0'
      cursor_.next();  // consume 'o'

      // consume octal digits
      while (true) {
        const char32_t c = cursor_.peek();
        if (core::is_ascii_octal_digit(c)) {
          meta.has_digit = true;
          cursor_.next();
        } else {
          break;
        }
      }

      // check for invalid octal digits (8-9)
      if (core::is_ascii_digit(cursor_.peek()) &&
          !core::is_ascii_octal_digit(cursor_.peek())) {
        return err<Token>(
            Error::create(start, line, col,
                          diagnostic::DiagnosticId::kInvalidNumericLiteral));
      }
    }
  }

  // handle decimal numbers (if not base-prefixed)
  if (!meta.is_base_prefixed) {
    // consume initial digits
    while (core::is_ascii_digit(cursor_.peek())) {
      meta.has_digit = true;
      cursor_.next();
    }

    // handle decimal point
    if (cursor_.peek() == '.' && core::is_ascii_digit(cursor_.peek_at(1))) {
      meta.seen_dot = true;
      cursor_.next();  // consume '.'

      // consume fractional digits
      while (core::is_ascii_digit(cursor_.peek())) {
        meta.has_digit = true;
        cursor_.next();
      }
    }

    // handle scientific notation
    if ((cursor_.peek() == 'e' || cursor_.peek() == 'E') && meta.has_digit) {
      meta.seen_exponent = true;
      cursor_.next();  // consume 'e'

      // optional sign
      if (cursor_.peek() == '+' || cursor_.peek() == '-') {
        cursor_.next();
      }

      // must have digits after exponent
      bool has_exp_digits = false;
      while (core::is_ascii_digit(cursor_.peek())) {
        has_exp_digits = true;
        cursor_.next();
      }

      if (!has_exp_digits) {
        return err<Token>(
            Error::create(start, line, col,
                          diagnostic::DiagnosticId::kInvalidNumericLiteral));
      }
    }
  }

  // check if we actually found any valid digits
  if (!meta.has_digit) {
    return err<Token>(Error::create(
        start, line, col, diagnostic::DiagnosticId::kInvalidNumericLiteral));
  }

  // handle optional suffix
  if (core::is_ascii_alphabet(cursor_.peek())) {
    const char32_t suffix = cursor_.peek();
    if (suffix == 'f' || suffix == 'd' || suffix == 'L') {
      cursor_.next();
    } else {
      // invalid suffix
      return err<Token>(Error::create(
          start, line, col, diagnostic::DiagnosticId::kInvalidNumericLiteral));
    }
  }

  return create_token(TokenKind::kLiteralNumeric, start, line, col);
}

}  // namespace lexer
