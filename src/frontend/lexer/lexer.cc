// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

#include "frontend/base/keyword/keyword.h"
#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/annotation.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/result.h"
#include "i18n/base/data/translation_key.h"
#include "unicode/base/unicode_util.h"
#include "unicode/utf8/file.h"

namespace lexer {

Lexer::Lexer(Mode mode) : mode_(mode) {}

Lexer::InitResult Lexer::init(const unicode::Utf8File& file) {
  std::size_t result = cursor_.init(file);

  if (result == 0) {
    return diagnostic::Result<std::size_t, diagnostic::DiagnosticEntry>(
        diagnostic::create_ok(result));
  } else {
    return diagnostic::Result<
        std::size_t, diagnostic::DiagnosticEntry>(diagnostic::create_err(
        std::move(
            diagnostic::EntryBuilder(diagnostic::Severity::kFatal,
                                     diagnostic::DiagId::kInvalidUtfSequence)
                .annotation(
                    diagnostic::AnnotationSeverity::kHelp,
                    i18n::TranslationKey::
                        kDiagnosticAnnotationChangeCharsetToUtf8AndTryAgain))
            .build()));
  }
}

Lexer::Result<Lexer::Token> Lexer::next_token() {
  skip_whitespace();
  if (cursor_.eof()) {
    return Result<Token>(diagnostic::create_ok(
        Token(TokenKind::kEof, cursor_.line(), cursor_.column(), 0)));
  }
  const char32_t current_codepoint = cursor_.peek();
  const std::size_t line = cursor_.line();
  const std::size_t col = cursor_.column();
  const std::size_t start = cursor_.position();

  if (unicode::is_eof(current_codepoint)) {
    return err<Token>(Error::create(
        line, col, 0, diagnostic::DiagnosticId::kUnexpectedEndOfFile));
  }

  // for ascii characters (most common case)
  if (unicode::is_ascii(current_codepoint)) {
    return ascii_token(static_cast<char>(current_codepoint), start, line, col);
  }

  // unicode-specific handling
  return unicode_token(current_codepoint, start, line, col);
}

Lexer::Results<Lexer::Token> Lexer::tokenize_all(bool strict) {
  std::vector<Token> tokens;
  tokens.reserve(kPredictedTokensCount);

  std::vector<Error> errors;

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
    return Results<Token>(diagnostic::create_err(std::move(errors)));
  }

  return Results<Token>(diagnostic::create_ok(std::move(tokens)));
}

void Lexer::skip_whitespace() {
  while (!cursor_.eof()) {
    if (unicode::is_unicode_whitespace(cursor_.peek())) {
      cursor_.next();
    } else {
      break;
    }
  }
}

Lexer::Result<Lexer::Token> Lexer::identifier_or_keyword() {
  const std::size_t start = cursor_.position();
  const std::size_t line = cursor_.line();
  const std::size_t col = cursor_.column();

  while (unicode::is_xid_continue(cursor_.peek())) {
    cursor_.next();
  }

  TokenKind kind = base::lookup_id_or_keyword(
      cursor_.file().content_u8(), start, cursor_.position() - start);
  return create_token(kind, start, line, col);
}

}  // namespace lexer
