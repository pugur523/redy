// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

#include <cctype>
#include <string>
#include <utility>
#include <vector>

#include "frontend/lexer/keyword/keyword.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

using FileManager = core::FileManager;
using FileId = core::FileId;

Lexer::Lexer(FileManager* file_manager, FileId file_id)
    : char_stream_(file_manager, file_id) {}

// Helper to check if a character is a valid identifier character after the
// first one
inline bool is_identifier_char(char c) {
  return std::isalnum(c) || c == '_';
}

Token Lexer::next_token() {
  skip_whitespace_and_comments();
  if (char_stream_.eof()) {
    return Token(TokenKind::kEof, char_stream_.file_id(), char_stream_.line(),
                 char_stream_.column(), 0);
  }

  char c = char_stream_.peek();
  std::size_t line = char_stream_.line();
  std::size_t col = char_stream_.column();
  std::size_t start = char_stream_.position();

  // Identifiers and Keywords
  if (std::isalpha(c) || c == '_') {
    return identifier_or_keyword();
  }

  // Number Literals
  if (std::isdigit(c)) {
    return literal_numeric();
  }

  // String Literals
  if (c == '"') {
    return literal_string();
  }

  // Character Literals
  if (c == '\'') {
    return literal_char();
  }

  // Consume the current character
  char_stream_.advance();

  switch (c) {
    // Single character tokens
    case ';': return make_token(TokenKind::kSemicolon, start, line, col);
    case ',': return make_token(TokenKind::kComma, start, line, col);
    case '(': return make_token(TokenKind::kLParen, start, line, col);
    case ')': return make_token(TokenKind::kRParen, start, line, col);
    case '{': return make_token(TokenKind::kLBrace, start, line, col);
    case '}': return make_token(TokenKind::kRBrace, start, line, col);
    case '[': return make_token(TokenKind::kLBracket, start, line, col);
    case ']': return make_token(TokenKind::kRBracket, start, line, col);
    case '@': return make_token(TokenKind::kAt, start, line, col);
    case '#': return make_token(TokenKind::kHash, start, line, col);
    case '$': return make_token(TokenKind::kDollar, start, line, col);
    case '?': return make_token(TokenKind::kQuestion, start, line, col);
    case '~': return make_token(TokenKind::kTilde, start, line, col);
    case '%':  // % or %=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPercentEq, start, line, col);
      } else {
        return make_token(TokenKind::kPercent, start, line, col);
      }
    case '&':  // & or && or &=
      if (char_stream_.peek() == '&') {
        char_stream_.advance();
        return make_token(TokenKind::kAndAnd, start, line, col);
      } else if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kAmpEq, start, line, col);
      } else {
        return make_token(TokenKind::kAmp, start, line, col);
      }
    case '|':  // | or || or |=
      if (char_stream_.peek() == '|') {
        char_stream_.advance();
        return make_token(TokenKind::kPipePipe, start, line, col);
      } else if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPipeEq, start, line, col);
      } else {
        return make_token(TokenKind::kPipe, start, line, col);
      }
    case '^':  // ^ or ^=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kCaretEq, start, line, col);
      } else {
        return make_token(TokenKind::kCaret, start, line, col);
      }

    // Multi-character token handling (longest match first)
    case '+':  // +, +=, ++
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPlusEq, start, line, col);
      } else if (char_stream_.peek() == '+') {
        char_stream_.advance();
        return make_token(TokenKind::kPlusPlus, start, line, col);
      } else {
        return make_token(TokenKind::kPlus, start, line, col);
      }
    case '-':  // -, ->, --, -=
      if (char_stream_.peek() == '>') {
        char_stream_.advance();
        return make_token(TokenKind::kArrow, start, line, col);
      } else if (char_stream_.peek() == '-') {
        char_stream_.advance();
        return make_token(TokenKind::kMinusMinus, start, line, col);
      } else if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kMinusEq, start, line, col);
      } else {
        return make_token(TokenKind::kMinus, start, line, col);
      }
    case '*':  // *, ** or *=
      if (char_stream_.peek() == '*') {
        char_stream_.advance();
        return make_token(TokenKind::kStarStar, start, line, col);
      } else if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kStarEq, start, line, col);
      } else {
        return make_token(TokenKind::kStar, start, line, col);
      }
    case '/':  // /, /= (and comments handled in skip_whitespace_and_comments)
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kSlashEq, start, line, col);
      } else {
        return make_token(TokenKind::kSlash, start, line, col);
      }
    case '=':  // =, ==
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kEqEq, start, line, col);
      } else {
        return make_token(TokenKind::kEqual, start, line, col);
      }
    case '!':  // !, !=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kNeq, start, line, col);
      } else {
        return make_token(TokenKind::kBang, start, line, col);
      }
    case '<':  // <, <=, <<, <<=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kLe, start, line, col);
      } else if (char_stream_.peek() == '<') {
        char_stream_.advance();
        if (char_stream_.peek() == '=') {  // <<=
          char_stream_.advance();
          return make_token(TokenKind::kLtLtEq, start, line, col);
        }
        return make_token(TokenKind::kLtLt, start, line, col);
      } else {
        return make_token(TokenKind::kLt, start, line, col);
      }
    case '>':  // >, >=, >>, >>=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kGe, start, line, col);
      } else if (char_stream_.peek() == '>') {
        char_stream_.advance();
        if (char_stream_.peek() == '=') {  // >>=
          char_stream_.advance();
          return make_token(TokenKind::kGtGtEq, start, line, col);
        }
        return make_token(TokenKind::kGtGt, start, line, col);
      } else {
        return make_token(TokenKind::kGt, start, line, col);
      }
    case ':':  // :, ::, :=
      if (char_stream_.peek() == ':') {
        char_stream_.advance();
        return make_token(TokenKind::kColonColon, start, line, col);
      } else if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kAssign, start, line, col);
      } else {
        return make_token(TokenKind::kColon, start, line, col);
      }
    case '.':  // ., ..
      if (char_stream_.peek() == '.') {
        char_stream_.advance();
        return make_token(TokenKind::kDotDot, start, line, col);
      } else {
        return make_token(TokenKind::kDot, start, line, col);
      }

    default: return make_token(TokenKind::kUnknown, start, line, col);
  }
}

std::vector<Token> Lexer::lex_all() {
  std::vector<Token> tokens;
  tokens.reserve(kPredictedTokensCount);

  while (true) {
    Token token = next_token();
    tokens.emplace_back(std::move(token));
    if (tokens.back().kind() == TokenKind::kEof) {
      break;
    }
  }
  return tokens;
}

void Lexer::skip_whitespace_and_comments() {
  while (!char_stream_.eof()) {
    char c = char_stream_.peek();

    if (std::isspace(c)) {
      char_stream_.advance();
    } else if (c == '/' && char_stream_.peek_ahead(1) == '/') {
      // Line comments //
      while (!char_stream_.eof() && char_stream_.peek() != '\n') {
        char_stream_.advance();
      }
      // Consume the newline character for single-line comments if present
      if (!char_stream_.eof() && char_stream_.peek() == '\n') {
        char_stream_.advance();
      }
    } else if (c == '/' && char_stream_.peek_ahead(1) == '*') {
      // Block comments /* ... */
      char_stream_.advance();  // Consume '/'
      char_stream_.advance();  // Consume '*'
      while (!char_stream_.eof()) {
        if (char_stream_.peek() == '*' && char_stream_.peek_ahead(1) == '/') {
          char_stream_.advance();  // Consume '*'
          char_stream_.advance();  // Consume '/'
          break;                   // End of block comment
        }
        char_stream_.advance();
      }
    } else {
      // Not whitespace or a comment, stop skipping
      break;
    }
  }
}

Token Lexer::identifier_or_keyword() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // Use the new helper to be consistent with identifier rules
  while (is_identifier_char(char_stream_.peek())) {
    char_stream_.advance();
  }

  TokenKind kind = lookup_identifier_or_keyword(
      char_stream_.file().source(), start, char_stream_.position() - start);
  return make_token(kind, start, line, col);
}

Token Lexer::literal_numeric() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // --- Handle base-prefixed literals ---
  if (char_stream_.peek() == '0') {
    // consume '0'
    char_stream_.advance();
    if (char_stream_.peek() == 'x' || char_stream_.peek() == 'X') {
      char_stream_.advance();
      while (std::isxdigit(char_stream_.peek())) {
        char_stream_.advance();
      }
      return make_token(TokenKind::kLiteralNumeric, start, line, col);
    } else if (char_stream_.peek() == 'b' || char_stream_.peek() == 'B') {
      char_stream_.advance();
      while (char_stream_.peek() == '0' || char_stream_.peek() == '1') {
        char_stream_.advance();
      }
      return make_token(TokenKind::kLiteralNumeric, start, line, col);
    } else if (char_stream_.peek() == 'o' || char_stream_.peek() == 'O') {
      char_stream_.advance();
      while (char_stream_.peek() >= '0' && char_stream_.peek() <= '7') {
        char_stream_.advance();
      }
      return make_token(TokenKind::kLiteralNumeric, start, line, col);
    } else {
      // Just a plain '0'
    }
  }

  // --- Handle decimal/floating-point ---
  while (std::isdigit(char_stream_.peek())) {
    char_stream_.advance();
  }

  if (char_stream_.peek() == '.' && std::isdigit(char_stream_.peek_ahead(1))) {
    char_stream_.advance();  // consume '.'
    while (std::isdigit(char_stream_.peek())) {
      char_stream_.advance();
    }
  }

  // Scientific notation
  if ((char_stream_.peek() == 'e' || char_stream_.peek() == 'E')) {
    char_stream_.advance();
    if (char_stream_.peek() == '+' || char_stream_.peek() == '-') {
      char_stream_.advance();
    }
    while (std::isdigit(char_stream_.peek())) {
      char_stream_.advance();
    }
  }

  // Optional suffix
  if (std::isalpha(char_stream_.peek())) {
    char suffix = char_stream_.peek();
    if (suffix == 'f' || suffix == 'd' || suffix == 'L') {
      // Optionally handle suffix type in future
      char_stream_.advance();
    }
  }

  return make_token(TokenKind::kLiteralNumeric, start, line, col);
}

Token Lexer::literal_string() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  char_stream_.advance();  // Consume the opening '"'

  // Consume characters until closing '"' or EOF
  while (!char_stream_.eof() && char_stream_.peek() != '"') {
    // Handle escape sequences like \" or \\ (basic example, could be more
    // robust)
    if (char_stream_.peek() == '\\') {
      // Consume '\'
      char_stream_.advance();
      if (!char_stream_.eof()) {
        // Consume escaped character
        char_stream_.advance();
      }
    } else {
      char_stream_.advance();
    }
  }

  if (char_stream_.eof()) {
    // Error: unclosed string literal
    return make_token(TokenKind::kUnknown, start, line, col);
  }

  // Consume the closing '"'
  char_stream_.advance();

  return make_token(TokenKind::kLiteralString, start, line, col);
}

Token Lexer::literal_char() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // Consume the opening '\''
  char_stream_.advance();

  // Consume character(s) inside quotes
  if (!char_stream_.eof() && char_stream_.peek() != '\'') {
    // Handle basic escape sequences like '\n', '\t', '\\', '\''
    if (char_stream_.peek() == '\\') {
      // Consume '\'
      char_stream_.advance();
      if (!char_stream_.eof()) {
        // Consume escaped character
        char_stream_.advance();
      }
    } else {
      char_stream_.advance();
    }
  }

  if (char_stream_.eof() || char_stream_.peek() != '\'') {
    // Error: unclosed character literal or empty char literal (e.g., '')
    return make_token(TokenKind::kUnknown, start, line, col);
  }

  // Consume the closing '\''
  char_stream_.advance();

  return make_token(TokenKind::kLiteralChar, start, line, col);
}

Token Lexer::make_token(TokenKind kind,
                        std::size_t start_pos,
                        std::size_t line,
                        std::size_t col) {
  const std::size_t end_pos = char_stream_.position();
  return Token(kind, char_stream_.file_id(), line, col, end_pos - start_pos);
}

}  // namespace lexer
