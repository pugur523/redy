// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

#include <charconv>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "frontend/base/keyword/keyword.h"
#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/result.h"
#include "unicode/base/unicode_util.h"

namespace lexer {

Lexer::Lexer(const core::File* file)
    : char_stream_(std::make_unique<CharStream>(file)) {
  DCHECK(char_stream_);
}

Lexer::Result<Lexer::Token> Lexer::next_token() {
  skip_whitespace();
  if (char_stream_->eof()) {
    return Result<Token>(
        diagnostic::make_ok(Token(TokenKind::kEof, char_stream_->line(),
                                  char_stream_->codepoint_column(), 0)));
  }
  const uint32_t current_codepoint = char_stream_->peek_codepoint();
  const std::size_t line = char_stream_->line();
  const std::size_t col = char_stream_->codepoint_column();
  const std::size_t start = char_stream_->position();

  if (unicode::is_eof(current_codepoint)) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnexpectedEndOfFile, line,
                       col, 0, "unexpected eof")));
  }

  if (!char_stream_->is_valid_utf8_current_codepoint()) {
    char_stream_->advance_codepoint();  // skip invalid sequence
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kInvalidUtfSequence, start,
                       line, col, "invalid utf8 sequence")));
  }

  // for ascii characters (most common case)
  if (unicode::is_ascii(current_codepoint)) {
    return ascii_token(static_cast<char>(current_codepoint), start, line, col);
  }

  // unicode-specific handling
  return unicode_token(current_codepoint, start, line, col);
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
  while (!char_stream_->eof()) {
    if (unicode::is_unicode_whitespace(char_stream_->peek_codepoint())) {
      char_stream_->advance_codepoint();
    } else {
      break;
    }
  }
}

Lexer::Result<Lexer::Token> Lexer::identifier_or_keyword() {
  const std::size_t start = char_stream_->position();
  const std::size_t line = char_stream_->line();
  const std::size_t col = char_stream_->column();

  while (unicode::is_xid_continue(char_stream_->peek_codepoint())) {
    char_stream_->advance_codepoint();
  }

  TokenKind kind = base::lookup_id_or_keyword(
      char_stream_->file().source(), start, char_stream_->position() - start);
  return make_token(kind, start, line, col);
}

}  // namespace lexer
