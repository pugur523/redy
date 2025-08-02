// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <cctype>
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
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  NumericMeta meta;

  // handle base prefixes (0x, 0b, 0o)
  if (char_stream_.peek() == '0') {
    const char next = char_stream_.peek(1);
    if (next == 'x' || next == 'X') {
      // hex
      meta.is_base_prefixed = true;
      char_stream_.advance();  // consume '0'
      char_stream_.advance();  // consume 'x'/'X'

      // consume hex digits
      while (std::isxdigit(char_stream_.peek())) {
        meta.has_digit = true;
        char_stream_.advance();
      }

    } else if (next == 'b' || next == 'B') {
      // binary
      meta.is_base_prefixed = true;
      char_stream_.advance();  // consume '0'
      char_stream_.advance();  // consume 'b'/'B'

      // consume binary digits
      while (char_stream_.peek() == '0' || char_stream_.peek() == '1') {
        meta.has_digit = true;
        char_stream_.advance();
      }

      // check for invalid binary digits (2-9)
      if (core::is_ascii_digit(char_stream_.peek()) &&
          char_stream_.peek() > '1') {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid binary literal")));
      }

    } else if (next == 'o' || next == 'O') {
      // octal
      meta.is_base_prefixed = true;
      char_stream_.advance();  // consume '0'
      char_stream_.advance();  // consume 'o'/'O'

      // consume octal digits
      while (char_stream_.peek() >= '0' && char_stream_.peek() <= '7') {
        meta.has_digit = true;
        char_stream_.advance();
      }

      // check for invalid octal digits (8-9)
      if (core::is_ascii_digit(char_stream_.peek()) &&
          char_stream_.peek() > '7') {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid octal literal")));
      }
    }
  }

  // handle decimal numbers (if not base-prefixed)
  if (!meta.is_base_prefixed) {
    // consume initial digits
    while (core::is_ascii_digit(char_stream_.peek())) {
      meta.has_digit = true;
      char_stream_.advance();
    }

    // handle decimal point
    if (char_stream_.peek() == '.' &&
        core::is_ascii_digit(char_stream_.peek(1))) {
      meta.seen_dot = true;
      char_stream_.advance();  // consume '.'

      // consume fractional digits
      while (core::is_ascii_digit(char_stream_.peek())) {
        meta.has_digit = true;
        char_stream_.advance();
      }
    }

    // handle scientific notation
    if ((char_stream_.peek() == 'e' || char_stream_.peek() == 'E') &&
        meta.has_digit) {
      meta.seen_exponent = true;
      char_stream_.advance();  // consume 'e'/'E'

      // optional sign
      if (char_stream_.peek() == '+' || char_stream_.peek() == '-') {
        char_stream_.advance();
      }

      // must have digits after exponent
      bool has_exp_digits = false;
      while (core::is_ascii_digit(char_stream_.peek())) {
        has_exp_digits = true;
        char_stream_.advance();
      }

      if (!has_exp_digits) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid scientific notation")));
      }
    }
  }

  // check if we actually found any valid digits
  if (!meta.has_digit) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral, start,
                       line, col, "invalid numeric literal")));
  }

  // handle optional suffix
  if (core::is_ascii_alphabet(char_stream_.peek())) {
    const char suffix = char_stream_.peek();
    if (suffix == 'f' || suffix == 'd' || suffix == 'L') {
      char_stream_.advance();
    } else {
      // invalid suffix
      return Result<Token>(diagnostic::make_err(
          LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                         start, line, col, "invalid numeric literal suffix")));
    }
  }

  return make_token(TokenKind::kLiteralNumeric, start, line, col);
}

}  // namespace lexer
