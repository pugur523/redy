// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

#include <cctype>
#include <string>
#include <utility>
#include <vector>

#include "frontend/base/keyword/keyword.h"
#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/result.h"

namespace lexer {

using FileManager = core::FileManager;
using FileId = core::FileId;

Lexer::Lexer(FileManager* file_manager, FileId file_id)
    : char_stream_(file_manager, file_id) {}

Lexer::Result<Lexer::Token> Lexer::next_token() {
  skip_whitespace();
  if (char_stream_.eof()) {
    return Result(diagnostic::make_ok(
        Token(TokenKind::kEof, char_stream_.file_id(), char_stream_.line(),
              char_stream_.column(), 0)));
  }

  const char c1 = char_stream_.peek();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();
  const std::size_t start = char_stream_.position();

  // identifiers and keywords
  if (std::isalpha(c1) || c1 == '_') {
    return identifier_or_keyword();
  }

  // numeric literals
  if (std::isdigit(c1)) {
    return literal_numeric();
  }

  // string literals
  if (c1 == '"') {
    return literal_str();
  }

  // character literals
  if (c1 == '\'') {
    return literal_char();
  }

  // consume the current character
  char_stream_.advance();
  const char c2 = char_stream_.peek();

  switch (c1) {
    // single character tokens
    case ';': return make_token(TokenKind::kSemicolon, start, line, col);
    case ',': return make_token(TokenKind::kComma, start, line, col);
    case '(': return make_token(TokenKind::kLeftParen, start, line, col);
    case ')': return make_token(TokenKind::kRightParen, start, line, col);
    case '{': return make_token(TokenKind::kLeftBrace, start, line, col);
    case '}': return make_token(TokenKind::kRightBrace, start, line, col);
    case '[': return make_token(TokenKind::kLeftBracket, start, line, col);
    case ']': return make_token(TokenKind::kRightBracket, start, line, col);
    case '@': return make_token(TokenKind::kAt, start, line, col);
    case '#': return make_token(TokenKind::kHash, start, line, col);
    case '$': return make_token(TokenKind::kDollar, start, line, col);
    case '?': return make_token(TokenKind::kQuestion, start, line, col);
    case '~': return make_token(TokenKind::kTilde, start, line, col);
    case '%':  // % or %=
      if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPercentEq, start, line, col);
      } else {
        return make_token(TokenKind::kPercent, start, line, col);
      }
    case '&':  // & or && or &=
      if (c2 == '&') {
        char_stream_.advance();
        return make_token(TokenKind::kAndAnd, start, line, col);
      } else if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kAndEq, start, line, col);
      } else {
        return make_token(TokenKind::kAnd, start, line, col);
      }
    case '|':  // | or || or |=
      if (c2 == '|') {
        char_stream_.advance();
        return make_token(TokenKind::kPipePipe, start, line, col);
      } else if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPipeEq, start, line, col);
      } else {
        return make_token(TokenKind::kPipe, start, line, col);
      }
    case '^':  // ^ or ^=
      if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kCaretEq, start, line, col);
      } else {
        return make_token(TokenKind::kCaret, start, line, col);
      }

    // multi-character token handling (longest match first)
    case '+':  // +, +=, ++
      if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kPlusEq, start, line, col);
      } else if (c2 == '+') {
        char_stream_.advance();
        return make_token(TokenKind::kPlusPlus, start, line, col);
      } else {
        return make_token(TokenKind::kPlus, start, line, col);
      }
    case '-':  // -, ->, --, -=
      if (c2 == '>') {
        char_stream_.advance();
        return make_token(TokenKind::kArrow, start, line, col);
      } else if (c2 == '-') {
        char_stream_.advance();
        return make_token(TokenKind::kMinusMinus, start, line, col);
      } else if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kMinusEq, start, line, col);
      } else {
        return make_token(TokenKind::kMinus, start, line, col);
      }
    case '*':  // *, ** or *=
      if (c2 == '*') {
        char_stream_.advance();
        return make_token(TokenKind::kStarStar, start, line, col);
      } else if (c2 == '=') {
        char_stream_.advance();
        return make_token(TokenKind::kStarEq, start, line, col);
      } else {
        return make_token(TokenKind::kStar, start, line, col);
      }
    case '/':  // /, //, /*, /=
      if (c2 == '/') {
        char_stream_.advance();  // consume second '/'
        // read until end of file or line
        while (!char_stream_.eof() && char_stream_.peek() != '\n') {
          char_stream_.advance();
        }
        return make_token(TokenKind::kInlineComment, start, line, col);
      } else if (c2 == '*') {
        char_stream_.advance();  // consume '*'
        while (!char_stream_.eof()) {
          if (char_stream_.peek() == '*' && char_stream_.peek(1) == '/') {
            char_stream_.advance();  // consume '*'
            char_stream_.advance();  // consume '/'
            return make_token(TokenKind::kBlockComment, start, line, col);
          }
          char_stream_.advance();
        }
        // reached eof before finding '*/'
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kUnterminatedBlockComment,
                           start, line, col, "unterminated block comment")));
      } else if (char_stream_.peek() == '=') {
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
        return make_token(TokenKind::kNotEqual, start, line, col);
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

Lexer::Results<Lexer::Token> Lexer::lex_all(bool strict) {
  std::vector<Token> tokens;
  tokens.reserve(kPredictedTokensCount);

  std::vector<LexError> errors;

  while (true) {
    Result<Token> result = next_token();
    if (result.is_err()) {
      errors.push_back(result.unwrap_err());
      if (strict) {
        break;
      }
    } else {
      Token token = std::move(result).unwrap();
      TokenKind kind = token.kind();
      tokens.push_back(std::move(token));
      if (kind == TokenKind::kEof) {
        break;
      }
    }
  }

  if (!errors.empty()) {
    return Results<Token>(diagnostic::make_err(std::move(errors)));
  }

  return Results<Token>(diagnostic::make_ok(std::move(tokens)));
}

void Lexer::skip_whitespace() {
  while (!char_stream_.eof()) {
    char c = char_stream_.peek();

    if (std::isspace(c)) {
      char_stream_.advance();
    } else {
      break;
    }
  }
}

Lexer::Result<Lexer::Token> Lexer::identifier_or_keyword() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // use the new helper to be consistent with identifier rules
  while (is_identifier_char(char_stream_.peek())) {
    char_stream_.advance();
  }

  TokenKind kind = base::lookup_id_or_keyword(
      char_stream_.file().source(), start, char_stream_.position() - start);
  return make_token(kind, start, line, col);
}

Lexer::Result<Lexer::Token> Lexer::literal_numeric() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // handle base-prefixed literals
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
      // just a plain '0'
    }
  }

  // handle decimal/floating-point
  while (std::isdigit(char_stream_.peek())) {
    char_stream_.advance();
  }

  if (char_stream_.peek() == '.' && std::isdigit(char_stream_.peek(1))) {
    char_stream_.advance();  // consume '.'
    while (std::isdigit(char_stream_.peek())) {
      char_stream_.advance();
    }
  }

  // scientific notation
  if ((char_stream_.peek() == 'e' || char_stream_.peek() == 'E')) {
    char_stream_.advance();
    if (char_stream_.peek() == '+' || char_stream_.peek() == '-') {
      char_stream_.advance();
    }
    while (std::isdigit(char_stream_.peek())) {
      char_stream_.advance();
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

Lexer::Result<Lexer::Token> Lexer::literal_str() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  char_stream_.advance();  // consume the opening '"'

  // consume characters until closing '"' or eof
  while (!char_stream_.eof() && char_stream_.peek() != '"') {
    // handle escape sequences like \" or \\ (basic example, could be more
    // robust)
    if (char_stream_.peek() == '\\') {
      // consume '\'
      char_stream_.advance();
      if (!char_stream_.eof()) {
        // consume escaped character
        char_stream_.advance();
      }
    } else {
      char_stream_.advance();
    }
  }

  if (char_stream_.eof()) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedStringLiteral,
                       start, line, col, "unterminated string literal")));
  }

  // consume the closing '"'
  char_stream_.advance();

  return make_token(TokenKind::kLiteralStr, start, line, col);
}

Lexer::Result<Lexer::Token> Lexer::literal_char() {
  const std::size_t start = char_stream_.position();
  const std::size_t line = char_stream_.line();
  const std::size_t col = char_stream_.column();

  // consume the opening '\''
  char_stream_.advance();

  // consume character(s) inside quotes
  if (!char_stream_.eof() && char_stream_.peek() != '\'') {
    // handle basic escape sequences like '\n', '\t', '\\', '\''
    if (char_stream_.peek() == '\\') {
      // consume '\'
      char_stream_.advance();
      if (!char_stream_.eof()) {
        // consume escaped character
        char_stream_.advance();
      }
    } else {
      char_stream_.advance();
    }
  }

  if (char_stream_.eof() || char_stream_.peek() != '\'') {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnterminatedCharacterLiteral,
                       start, line, col, "unterminated character literal")));
  }

  // consume the closing '\''
  char_stream_.advance();

  return make_token(TokenKind::kLiteralChar, start, line, col);
}

inline Lexer::Result<Lexer::Token> Lexer::make_token(TokenKind kind,
                                                     std::size_t start_pos,
                                                     std::size_t line,
                                                     std::size_t column) {
  const std::size_t end_pos = char_stream_.position();
  const FileId file_id = char_stream_.file_id();
  const std::size_t length = end_pos - start_pos;
  return Result(
      diagnostic::make_ok(Token(kind, file_id, line, column, length)));
}

}  // namespace lexer
