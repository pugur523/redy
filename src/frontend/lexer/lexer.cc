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
    return Token(TokenKind::kEof, char_stream_.file_manager(),
                 char_stream_.file_id(), char_stream_.line(),
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
    return literal_number();
  }

  // String Literals
  if (c == '"') {
    return literal_string();
  }

  // Character Literals
  if (c == '\'') {
    return literal_char();
  }

  char_stream_.advance();  // Consume the current character

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
      }
      return make_token(TokenKind::kPercent, start, line, col);
    case '&':  // & or && or &=
      if (char_stream_.peek() == '&') {
        char_stream_.advance();
        return make_token(TokenKind::kAndAnd, start, line, col);
      }
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kAmpEq, start, line, col);
      }
      return make_token(TokenKind::kAmp, start, line, col);
    case '|':  // | or || or |=
      if (char_stream_.peek() == '|') {
        char_stream_.advance();
        return make_token(TokenKind::kPipePipe, start, line, col);
      }
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPipeEq, start, line, col);
      }
      return make_token(TokenKind::kPipe, start, line, col);
    case '^':  // ^ or ^=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kCaretEq, start, line, col);
      }
      return make_token(TokenKind::kCaret, start, line, col);

    // Multi-character token handling (longest match first)
    case '+':  // +, +=, ++
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPlusEq, start, line, col);
      }
      if (char_stream_.peek() == '+') {
        char_stream_.advance();
        return make_token(TokenKind::kPlusPlus, start, line, col);
      }
      return make_token(TokenKind::kPlus, start, line, col);
    case '-':  // -, ->, --, -=
      if (char_stream_.peek() == '>') {
        char_stream_.advance();
        return make_token(TokenKind::kArrow, start, line, col);
      }
      if (char_stream_.peek() == '-') {
        char_stream_.advance();
        return make_token(TokenKind::kMinusMinus, start, line, col);
      }
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kMinusEq, start, line, col);
      }
      return make_token(TokenKind::kMinus, start, line, col);
    case '*':  // *, ** or *=
      if (char_stream_.peek() == '*') {
        char_stream_.advance();
        return make_token(TokenKind::kStarStar, start, line, col);
      }
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kStarEq, start, line, col);
      }
      return make_token(TokenKind::kStar, start, line, col);
    case '/':  // /, /= (and comments handled in skip_whitespace_and_comments)
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kSlashEq, start, line, col);
      }
      return make_token(TokenKind::kSlash, start, line, col);
    case '=':  // =, ==
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kEqEq, start, line, col);
      }
      return make_token(TokenKind::kAssign, start, line, col);
    case '!':  // !, !=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kNeq, start, line, col);
      }
      return make_token(TokenKind::kBang, start, line, col);
    case '<':  // <, <=, <<, <<=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kLe, start, line, col);
      }
      if (char_stream_.peek() == '<') {
        char_stream_.advance();
        if (char_stream_.peek() == '=') {  // <<=
          char_stream_.advance();
          return make_token(TokenKind::kLtLtEq, start, line, col);
        }
        return make_token(TokenKind::kLtLt, start, line, col);
      }
      return make_token(TokenKind::kLt, start, line, col);
    case '>':  // >, >=, >>, >>=
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kGe, start, line, col);
      }
      if (char_stream_.peek() == '>') {
        char_stream_.advance();
        if (char_stream_.peek() == '=') {  // >>=
          char_stream_.advance();
          return make_token(TokenKind::kGtGtEq, start, line, col);
        }
        return make_token(TokenKind::kGtGt, start, line, col);
      }
      return make_token(TokenKind::kGt, start, line, col);
    case ':':  // :, ::, :=
      if (char_stream_.peek() == ':') {
        char_stream_.advance();
        return make_token(TokenKind::kColonColon, start, line, col);
      }
      if (char_stream_.peek() == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kLet, start, line, col);
      }
      return make_token(TokenKind::kColon, start, line, col);
    case '.':  // ., ..
      if (char_stream_.peek() == '.') {
        char_stream_.advance();
        return make_token(TokenKind::kDotDot, start, line, col);
      }
      return make_token(TokenKind::kDot, start, line, col);

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
    } else if (c == '/' &&
               char_stream_.peek_ahead(1) == '/') {  // Line comments //
      while (!char_stream_.eof() && char_stream_.peek() != '\n') {
        char_stream_.advance();
      }
      // Consume the newline character for single-line comments if present
      if (!char_stream_.eof() && char_stream_.peek() == '\n') {
        char_stream_.advance();
      }
    } else if (c == '/' &&
               char_stream_.peek_ahead(1) == '*') {  // Block comments /* ... */
      char_stream_.advance();                        // Consume '/'
      char_stream_.advance();                        // Consume '*'
      while (!char_stream_.eof()) {
        if (char_stream_.peek() == '*' && char_stream_.peek_ahead(1) == '/') {
          char_stream_.advance();  // Consume '*'
          char_stream_.advance();  // Consume '/'
          break;                   // End of block comment
        }
        char_stream_.advance();
      }
    } else {
      break;  // Not whitespace or a comment, stop skipping
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

Token Lexer::literal_number() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // Consume digits for integer part
  while (std::isdigit(char_stream_.peek())) {
    char_stream_.advance();
  }

  // Handle floating-point numbers (e.g., 123.45)
  // Assumes only one dot for now. More complex rules (e.g., 1e-5) would need
  // more logic.
  if (char_stream_.peek() == '.' && std::isdigit(char_stream_.peek_ahead(1))) {
    char_stream_.advance();  // Consume '.'
    while (std::isdigit(char_stream_.peek())) {
      char_stream_.advance();  // Consume digits after '.'
    }
  }
  // TODO: Handle suffixes like f, d, L for floats/longs (e.g., 123.45f, 100L)
  // TODO: Handle scientific notation (e.g., 1e-5, 1.23E+10)
  // TODO: Handle hexadecimal (0x...), octal (0...), binary (0b...) literals

  return make_token(TokenKind::kLiteralNumber, start, line, col);
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
      char_stream_.advance();  // Consume '\'
      if (!char_stream_.eof()) {
        char_stream_.advance();  // Consume escaped character
      }
    } else {
      char_stream_.advance();
    }
  }

  if (char_stream_.eof()) {
    // Error: unclosed string literal
    return make_token(TokenKind::kUnknown, start, line, col);
  }

  char_stream_.advance();  // Consume the closing '"'

  return make_token(TokenKind::kLiteralString, start, line, col);
}

Token Lexer::literal_char() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  char_stream_.advance();  // Consume the opening '\''

  // Consume character(s) inside quotes
  if (!char_stream_.eof() && char_stream_.peek() != '\'') {
    if (char_stream_.peek() ==
        '\\') {  // Handle basic escape sequences like '\n', '\t', '\\', '\''
      char_stream_.advance();  // Consume '\'
      if (!char_stream_.eof()) {
        char_stream_.advance();  // Consume escaped character
      }
    } else {
      char_stream_.advance();
    }
  }

  if (char_stream_.eof() || char_stream_.peek() != '\'') {
    // Error: unclosed character literal or empty char literal (e.g., '')
    return make_token(TokenKind::kUnknown, start, line, col);
  }

  char_stream_.advance();  // Consume the closing '\''

  return make_token(TokenKind::kLiteralChar, start, line, col);
}

Token Lexer::make_token(TokenKind kind,
                        std::size_t start_pos,
                        std::size_t line,
                        std::size_t col) {
  const std::size_t end_pos = char_stream_.position();
  return Token(kind, char_stream_.file_manager(), char_stream_.file_id(), line,
               col, end_pos - start_pos);
}

}  // namespace lexer
