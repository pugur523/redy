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

Lexer::Lexer(const core::File& file)
    : stream_(std::make_unique<unicode::Utf8Stream>()) {
  DCHECK(stream_);
  [[maybe_unused]] std::size_t result = stream_->init(file);
  DCHECK(stream_->valid());
  DCHECK_EQ(result, 0);
}

Lexer::Result<Lexer::Token> Lexer::next_token() {
  skip_whitespace();
  if (stream_->eof()) {
    return Result<Token>(diagnostic::make_ok(
        Token(TokenKind::kEof, stream_->line(), stream_->column(), 0)));
  }
  const uint32_t current_codepoint = stream_->peek();
  const std::size_t line = stream_->line();
  const std::size_t col = stream_->column();
  const std::size_t start = stream_->position();

  if (unicode::is_eof(current_codepoint)) {
    return Result<Token>(diagnostic::make_err(
        LexError::make(diagnostic::DiagnosticId::kUnexpectedEndOfFile, line,
                       col, 0, "unexpected eof")));
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
  while (!stream_->eof()) {
    if (unicode::is_unicode_whitespace(stream_->peek())) {
      stream_->advance();
    } else {
      break;
    }
  }
}

Lexer::Result<Lexer::Token> Lexer::identifier_or_keyword() {
  const std::size_t start = stream_->position();
  const std::size_t line = stream_->line();
  const std::size_t col = stream_->column();

  while (unicode::is_xid_continue(stream_->peek())) {
    stream_->advance();
  }

  TokenKind kind = base::lookup_id_or_keyword(stream_->file().source(), start,
                                              stream_->position() - start);
  return make_token(kind, start, line, col);
}

}  // namespace lexer
