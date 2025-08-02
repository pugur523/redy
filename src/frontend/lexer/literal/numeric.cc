// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <climits>
#include <cstdint>
#include <string>

#include "frontend/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::literal_numeric() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  std::string literal_value;
  bool is_floating_point = false;
  int base = 10;

  // handle base-prefixed literals
  if (char_stream_.peek() == '0') {
    literal_value += char_stream_.peek();
    char_stream_.advance();

    char next = char_stream_.peek();
    if (next == 'x' || next == 'X') {
      // hexadecimal
      base = 16;
      literal_value += next;
      char_stream_.advance();

      bool has_digits = false;
      while (std::isxdigit(char_stream_.peek())) {
        literal_value += char_stream_.peek();
        char_stream_.advance();
        has_digits = true;
      }

      if (!has_digits) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid hexadecimal literal")));
      }

    } else if (next == 'b' || next == 'B') {
      // binary
      base = 2;
      literal_value += next;
      char_stream_.advance();

      bool has_digits = false;
      while (char_stream_.peek() == '0' || char_stream_.peek() == '1') {
        literal_value += char_stream_.peek();
        char_stream_.advance();
        has_digits = true;
      }

      // check for invalid binary digits
      if (std::isdigit(char_stream_.peek()) && char_stream_.peek() > '1') {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid binary literal")));
      }

      if (!has_digits) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid binary literal")));
      }

    } else if (next == 'o' || next == 'O') {
      // octal
      base = 8;
      literal_value += next;
      char_stream_.advance();

      bool has_digits = false;
      while (char_stream_.peek() >= '0' && char_stream_.peek() <= '7') {
        literal_value += char_stream_.peek();
        char_stream_.advance();
        has_digits = true;
      }

      // check for invalid octal digits
      if (std::isdigit(char_stream_.peek()) && char_stream_.peek() > '7') {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid octal literal")));
      }

      if (!has_digits) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid octal literal")));
      }

    } else {
      // just a plain '0' or decimal starting with 0
      // continue to decimal handling
    }
  }

  // handle decimal/floating-point (if not already handled base-prefixed)
  if (base == 10) {
    while (std::isdigit(char_stream_.peek())) {
      literal_value += char_stream_.peek();
      char_stream_.advance();
    }

    if (char_stream_.peek() == '.' && std::isdigit(char_stream_.peek(1))) {
      is_floating_point = true;
      literal_value += char_stream_.peek();
      char_stream_.advance();
      while (std::isdigit(char_stream_.peek())) {
        literal_value += char_stream_.peek();
        char_stream_.advance();
      }
    }

    // scientific notation
    if ((char_stream_.peek() == 'e' || char_stream_.peek() == 'E')) {
      is_floating_point = true;
      literal_value += char_stream_.peek();
      char_stream_.advance();
      if (char_stream_.peek() == '+' || char_stream_.peek() == '-') {
        literal_value += char_stream_.peek();
        char_stream_.advance();
      }

      bool has_exp_digits = false;
      while (std::isdigit(char_stream_.peek())) {
        literal_value += char_stream_.peek();
        char_stream_.advance();
        has_exp_digits = true;
      }

      if (!has_exp_digits) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral,
                           start, line, col, "invalid scientific notation")));
      }
    }
  }

  // check for invalid characters immediately following the number
  if (std::isalpha(char_stream_.peek()) && char_stream_.peek() != 'f' &&
      char_stream_.peek() != 'd' && char_stream_.peek() != 'L') {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kInvalidNumericLiteral, start,
                       line, col, "invalid numeric literal")));
  }

  // range check for integer literals (skip for floating point)
  if (!is_floating_point && base != 10) {
    try {
      // extract the numeric part (skip prefix like "0x", "0b", "0o")
      std::string numeric_part = literal_value;
      if (base == 16 && literal_value.length() > 2) {
        numeric_part = literal_value.substr(2);
      } else if ((base == 2 || base == 8) && literal_value.length() > 2) {
        numeric_part = literal_value.substr(2);
      }

      if (!numeric_part.empty()) {
        // convert to check range
        uint64_t value = std::stoull(numeric_part, nullptr, base);

        // check if value exceeds usize max (assume 64-bit for now)
        const uint64_t USIZE_MAX = UINT64_MAX;
        if (value > USIZE_MAX) {
          return Result<Token>(diagnostic::make_err(LexError::make(
              diagnostic::DiagnosticId::kNumericLiteralOutOfRange, start, line,
              col, "numeric literal out of range")));
        }
      }
    } catch (const std::exception&) {
      return Result<Token>(diagnostic::make_err(
          LexError::make(diagnostic::DiagnosticId::kNumericLiteralOutOfRange,
                         start, line, col, "numeric literal out of range")));
    }
  } else if (!is_floating_point && base == 10) {
    try {
      // for decimal integers, check both signed and unsigned ranges
      int64_t signed_value = std::stoll(literal_value);
      uint64_t unsigned_value = std::stoull(literal_value);

      const int64_t kIsizeMin = LLONG_MIN;
      const uint64_t kUsizeMax = UINT64_MAX;

      // check if it's outside both signed and unsigned ranges
      if (signed_value < kIsizeMin || unsigned_value > kUsizeMax) {
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kNumericLiteralOutOfRange,
                           start, line, col, "numeric literal out of range")));
      }
    } catch (const std::exception&) {
      return Result<Token>(diagnostic::make_err(
          LexError::make(diagnostic::DiagnosticId::kNumericLiteralOutOfRange,
                         start, line, col, "numeric literal out of range")));
    }
  }

  // optional suffix
  if (std::isalpha(char_stream_.peek())) {
    char suffix = char_stream_.peek();
    if (suffix == 'f' || suffix == 'd' || suffix == 'L') {
      // optionally handle suffix type in future
      char_stream_.advance();
    }
  }

  return make_token(TokenKind::kLiteralNumeric, start, line, col);
}

}  // namespace lexer
