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
  const std::size_t start = char_stream_->position();
  const std::size_t line = char_stream_->line();
  const std::size_t col = char_stream_->column();

  NumericMeta meta;

  const char32_t ch0 = char_stream_->peek_codepoint();
  const char32_t ch1 = char_stream_->peek_codepoint(1);

  // base prefixes (0x, 0b, 0o)
  if (ch0 == U'0') {
    if (ch1 == U'x' || ch1 == U'X') {
      // hex
      meta.is_base_prefixed = true;
      char_stream_->advance_codepoint();  // consume '0'
      char_stream_->advance_codepoint();  // consume 'x'

      // consume hex digits
      while (core::is_ascii_hex_digit(char_stream_->peek_codepoint())) {
        meta.has_digit = true;
        char_stream_->advance_codepoint();
      }

    } else if (ch1 == U'b' || ch1 == U'B') {
      // binary
      meta.is_base_prefixed = true;
      char_stream_->advance_codepoint();  // consume '0'
      char_stream_->advance_codepoint();  // consume 'b'

      // consume binary digits
      while (true) {
        const char32_t c = char_stream_->peek_codepoint();
        if (core::is_ascii_binary_digit(c)) {
          meta.has_digit = true;
          char_stream_->advance_codepoint();
        } else {
          break;
        }
      }

      // check for invalid binary digits (2-9)
      if (core::is_ascii_digit(char_stream_->peek_codepoint()) &&
          !core::is_ascii_binary_digit(char_stream_->peek_codepoint())) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid binary literal")));
      }

    } else if (ch1 == U'o' || ch1 == U'O') {
      // octal
      meta.is_base_prefixed = true;
      char_stream_->advance_codepoint();  // consume '0'
      char_stream_->advance_codepoint();  // consume 'o'

      // consume octal digits
      while (true) {
        const char32_t c = char_stream_->peek_codepoint();
        if (core::is_ascii_octal_digit(c)) {
          meta.has_digit = true;
          char_stream_->advance_codepoint();
        } else {
          break;
        }
      }

      // check for invalid octal digits (8-9)
      if (core::is_ascii_digit(char_stream_->peek_codepoint()) &&
          !core::is_ascii_octal_digit(char_stream_->peek_codepoint())) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid octal literal")));
      }
    }
  }

  // handle decimal numbers (if not base-prefixed)
  if (!meta.is_base_prefixed) {
    // consume initial digits
    while (core::is_ascii_digit(char_stream_->peek_codepoint())) {
      meta.has_digit = true;
      char_stream_->advance_codepoint();
    }

    // handle decimal point
    if (char_stream_->peek_codepoint() == U'.' &&
        core::is_ascii_digit(char_stream_->peek_codepoint(1))) {
      meta.seen_dot = true;
      char_stream_->advance_codepoint();  // consume '.'

      // consume fractional digits
      while (core::is_ascii_digit(char_stream_->peek_codepoint())) {
        meta.has_digit = true;
        char_stream_->advance_codepoint();
      }
    }

    // handle scientific notation
    if ((char_stream_->peek_codepoint() == U'e' ||
         char_stream_->peek_codepoint() == U'E') &&
        meta.has_digit) {
      meta.seen_exponent = true;
      char_stream_->advance_codepoint();  // consume 'e'

      // optional sign
      if (char_stream_->peek_codepoint() == U'+' ||
          char_stream_->peek_codepoint() == U'-') {
        char_stream_->advance_codepoint();
      }

      // must have digits after exponent
      bool has_exp_digits = false;
      while (core::is_ascii_digit(char_stream_->peek_codepoint())) {
        has_exp_digits = true;
        char_stream_->advance_codepoint();
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
  if (core::is_ascii_alphabet(char_stream_->peek_codepoint())) {
    const char32_t suffix = char_stream_->peek_codepoint();
    if (suffix == U'f' || suffix == U'd' || suffix == U'L') {
      char_stream_->advance_codepoint();
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
