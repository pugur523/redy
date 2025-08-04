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
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kPercentEq, start, line, col);
      } else {
        return make_token(TokenKind::kPercent, start, line, col);
      }
    case '&':  // & or && or &=
      if (next_char == '&') {
        char_stream_->advance();
        return make_token(TokenKind::kAndAnd, start, line, col);
      } else if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kAndEq, start, line, col);
      } else {
        return make_token(TokenKind::kAnd, start, line, col);
      }
    case '|':  // | or || or |=
      if (next_char == '|') {
        char_stream_->advance();
        return make_token(TokenKind::kPipePipe, start, line, col);
      } else if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kPipeEq, start, line, col);
      } else {
        return make_token(TokenKind::kPipe, start, line, col);
      }
    case '^':  // ^ or ^=
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kCaretEq, start, line, col);
      } else {
        return make_token(TokenKind::kCaret, start, line, col);
      }

    // multi-character token handling (longest match first)
    case '+':  // +, +=, ++
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kPlusEq, start, line, col);
      } else if (next_char == '+') {
        char_stream_->advance();
        return make_token(TokenKind::kPlusPlus, start, line, col);
      } else {
        return make_token(TokenKind::kPlus, start, line, col);
      }
    case '-':  // -, ->, --, -=
      if (next_char == '>') {
        char_stream_->advance();
        return make_token(TokenKind::kArrow, start, line, col);
      } else if (next_char == '-') {
        char_stream_->advance();
        return make_token(TokenKind::kMinusMinus, start, line, col);
      } else if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kMinusEq, start, line, col);
      } else {
        return make_token(TokenKind::kMinus, start, line, col);
      }
    case '*':  // *, ** or *=
      if (next_char == '*') {
        char_stream_->advance();
        return make_token(TokenKind::kStarStar, start, line, col);
      } else if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kStarEq, start, line, col);
      } else {
        return make_token(TokenKind::kStar, start, line, col);
      }
    case '/':  // /, //, /*, /=
      if (next_char == '/') {
        char_stream_->advance();  // consume second '/'
        // read until end of file or line
        while (!char_stream_->eof() && char_stream_->peek() != '\n') {
          char_stream_->advance();
        }
        return make_token(TokenKind::kInlineComment, start, line, col);
      } else if (next_char == '*') {
        char_stream_->advance();  // consume '*'
        while (!char_stream_->eof()) {
          if (char_stream_->peek() == '*' && char_stream_->peek(1) == '/') {
            char_stream_->advance();  // consume '*'
            char_stream_->advance();  // consume '/'
            return make_token(TokenKind::kBlockComment, start, line, col);
          }
          char_stream_->advance();
        }
        // reached eof before finding '*/'
        return Result<Token>(diagnostic::make_err(
            LexError::make(diagnostic::DiagnosticId::kUnterminatedBlockComment,
                           start, line, col, "unterminated block comment")));
      } else if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kSlashEq, start, line, col);
      } else {
        return make_token(TokenKind::kSlash, start, line, col);
      }
    case '=':  // =, ==
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kEqEq, start, line, col);
      } else {
        return make_token(TokenKind::kEqual, start, line, col);
      }
    case '!':  // !, !=
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kNotEqual, start, line, col);
      } else {
        return make_token(TokenKind::kBang, start, line, col);
      }
    case '<':  // <, <=, <<, <<=
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kLe, start, line, col);
      } else if (next_char == '<') {
        char_stream_->advance();
        if (next_char == '=') {  // <<=
          char_stream_->advance();
          return make_token(TokenKind::kLtLtEq, start, line, col);
        }
        return make_token(TokenKind::kLtLt, start, line, col);
      } else {
        return make_token(TokenKind::kLt, start, line, col);
      }
    case '>':  // >, >=, >>, >>=
      if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kGe, start, line, col);
      } else if (next_char == '>') {
        char_stream_->advance();
        if (next_char == '=') {  // >>=
          char_stream_->advance();
          return make_token(TokenKind::kGtGtEq, start, line, col);
        }
        return make_token(TokenKind::kGtGt, start, line, col);
      } else {
        return make_token(TokenKind::kGt, start, line, col);
      }
    case ':':  // :, ::, :=
      if (next_char == ':') {
        char_stream_->advance();
        return make_token(TokenKind::kColonColon, start, line, col);
      } else if (next_char == '=') {
        char_stream_->advance();
        return make_token(TokenKind::kAssign, start, line, col);
      } else {
        return make_token(TokenKind::kColon, start, line, col);
      }
    case '.':  // ., ..
      if (next_char == '.') {
        char_stream_->advance();
        return make_token(TokenKind::kDotDot, start, line, col);
      } else {
        return make_token(TokenKind::kDot, start, line, col);
      }

    default:
      return Result<Token>(diagnostic::make_err(
          LexError::make(diagnostic::DiagnosticId::kUnrecognizedCharacter,
                         start, line, col, "unrecognized character")));
  }
}

}  // namespace lexer
