// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

namespace lexer {

Lexer::Result<Lexer::Token> Lexer::other_token(char current_char,
                                               char next_char,
                                               std::size_t start,
                                               std::size_t line,
                                               std::size_t col) {
  switch (current_char) {
    // single character tokens
    case ';': return create_token(TokenKind::kSemicolon, start, line, col);
    case ',': return create_token(TokenKind::kComma, start, line, col);
    case '(': return create_token(TokenKind::kLeftParen, start, line, col);
    case ')': return create_token(TokenKind::kRightParen, start, line, col);
    case '{': return create_token(TokenKind::kLeftBrace, start, line, col);
    case '}': return create_token(TokenKind::kRightBrace, start, line, col);
    case '[': return create_token(TokenKind::kLeftBracket, start, line, col);
    case ']': return create_token(TokenKind::kRightBracket, start, line, col);
    case '@': return create_token(TokenKind::kAt, start, line, col);
    case '#': return create_token(TokenKind::kHash, start, line, col);
    case '$': return create_token(TokenKind::kDollar, start, line, col);
    case '?': return create_token(TokenKind::kQuestion, start, line, col);
    case '~': return create_token(TokenKind::kTilde, start, line, col);
    case '%':  // % or %=
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kPercentEq, start, line, col);
      } else {
        return create_token(TokenKind::kPercent, start, line, col);
      }
    case '&':  // & or && or &=
      if (next_char == '&') {
        cursor_.next();
        return create_token(TokenKind::kAndAnd, start, line, col);
      } else if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kAndEq, start, line, col);
      } else {
        return create_token(TokenKind::kAnd, start, line, col);
      }
    case '|':  // | or || or |=
      if (next_char == '|') {
        cursor_.next();
        return create_token(TokenKind::kPipePipe, start, line, col);
      } else if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kPipeEq, start, line, col);
      } else {
        return create_token(TokenKind::kPipe, start, line, col);
      }
    case '^':  // ^ or ^=
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kCaretEq, start, line, col);
      } else {
        return create_token(TokenKind::kCaret, start, line, col);
      }

    // multi-character token handling (longest match first)
    case '+':  // +, +=, ++
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kPlusEq, start, line, col);
      } else if (next_char == '+') {
        cursor_.next();
        return create_token(TokenKind::kPlusPlus, start, line, col);
      } else {
        return create_token(TokenKind::kPlus, start, line, col);
      }
    case '-':  // -, ->, --, -=
      if (next_char == '>') {
        cursor_.next();
        return create_token(TokenKind::kArrow, start, line, col);
      } else if (next_char == '-') {
        cursor_.next();
        return create_token(TokenKind::kMinusMinus, start, line, col);
      } else if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kMinusEq, start, line, col);
      } else {
        return create_token(TokenKind::kMinus, start, line, col);
      }
    case '*':  // *, ** or *=
      if (next_char == '*') {
        cursor_.next();
        return create_token(TokenKind::kStarStar, start, line, col);
      } else if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kStarEq, start, line, col);
      } else {
        return create_token(TokenKind::kStar, start, line, col);
      }
    case '/':  // /, //, /*, /=
      if (next_char == '/') {
        cursor_.next();  // consume second '/'
        // read until end of file or line
        while (!cursor_.eof() && cursor_.peek() != '\n') {
          cursor_.next();
        }
        return create_token(TokenKind::kInlineComment, start, line, col);
      } else if (next_char == '*') {
        cursor_.next();  // consume '*'
        while (!cursor_.eof()) {
          if (cursor_.peek() == '*' && cursor_.peek_at(1) == '/') {
            cursor_.next();  // consume '*'
            cursor_.next();  // consume '/'
            return create_token(TokenKind::kBlockComment, start, line, col);
          }
          cursor_.next();
        }
        // reached eof before finding '*/'
        return err<Token>(
            Error::create(start, line, col,
                          diagnostic::DiagnosticId::kUnterminatedBlockComment));
      } else if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kSlashEq, start, line, col);
      } else {
        return create_token(TokenKind::kSlash, start, line, col);
      }
    case '=':  // =, ==
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kEqEq, start, line, col);
      } else {
        return create_token(TokenKind::kEqual, start, line, col);
      }
    case '!':  // !, !=
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kNotEqual, start, line, col);
      } else {
        return create_token(TokenKind::kBang, start, line, col);
      }
    case '<':  // <, <=, <<, <<=
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kLe, start, line, col);
      } else if (next_char == '<') {
        cursor_.next();
        if (next_char == '=') {  // <<=
          cursor_.next();
          return create_token(TokenKind::kLtLtEq, start, line, col);
        }
        return create_token(TokenKind::kLtLt, start, line, col);
      } else {
        return create_token(TokenKind::kLt, start, line, col);
      }
    case '>':  // >, >=, >>, >>=
      if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kGe, start, line, col);
      } else if (next_char == '>') {
        cursor_.next();
        if (next_char == '=') {  // >>=
          cursor_.next();
          return create_token(TokenKind::kGtGtEq, start, line, col);
        }
        return create_token(TokenKind::kGtGt, start, line, col);
      } else {
        return create_token(TokenKind::kGt, start, line, col);
      }
    case ':':  // :, ::, :=
      if (next_char == ':') {
        cursor_.next();
        return create_token(TokenKind::kColonColon, start, line, col);
      } else if (next_char == '=') {
        cursor_.next();
        return create_token(TokenKind::kAssign, start, line, col);
      } else {
        return create_token(TokenKind::kColon, start, line, col);
      }
    case '.':  // ., ..
      if (next_char == '.') {
        cursor_.next();
        return create_token(TokenKind::kDotDot, start, line, col);
      } else {
        return create_token(TokenKind::kDot, start, line, col);
      }

    default:
      return err<Token>(Error::create(
          start, line, col, diagnostic::DiagnosticId::kUnrecognizedCharacter));
  }
}

}  // namespace lexer
