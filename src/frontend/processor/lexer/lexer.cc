// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/lexer/lexer.h"

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

Lexer::Lexer() = default;

Lexer::InitResult Lexer::init(const unicode::Utf8File& file, Mode mode) {
  mode_ = mode;

  std::size_t result = cursor_.init(file);

  if (result == 0) {
    return InitResult(diagnostic::create_ok());
  } else {
    return InitResult(diagnostic::create_err(
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

Lexer::Results<Lexer::Token> Lexer::tokenize(bool strict) {
  std::vector<Token> tokens;
  tokens.reserve(cursor_.file().line_count() * kPredictedTokensCountPerLine);

  std::vector<Error> errors;

  while (true) {
    Result<Token> result = tokenize_next();
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

Lexer::Result<Lexer::Token> Lexer::tokenize_next() {
  const auto r = skip_trivia();
  if (r.is_err()) {
    return Result<Lexer::Token>(
        diagnostic::create_err(std::move(r).unwrap_err()));
  }

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

void Lexer::skip_whitespace() {
  while (!cursor_.eof() && !unicode::is_unicode_newline(cursor_.peek()) &&
         unicode::is_unicode_whitespace(cursor_.peek())) {
    cursor_.next();
  }
}

Lexer::Result<void> Lexer::skip_comments() {
  while (!cursor_.eof()) {
    if (cursor_.peek() == '/' && cursor_.peek_at(1) == '/') {
      const char32_t third_cp = cursor_.peek_at(2);
      if (third_cp == '@') {
        if (should_include_documentation_comments()) {
          break;
        }
      } else {
        if (should_include_normal_comments()) {
          break;
        }
      }
      // skip inline comment
      cursor_.next();
      cursor_.next();  // skip //
      while (!cursor_.eof() && !unicode::is_unicode_newline(cursor_.peek())) {
        cursor_.next();
      }
      continue;
    }

    if (cursor_.peek() == '/' && cursor_.peek_at(1) == '*') {
      if (should_include_normal_comments()) {
        break;
      }
      // skip block comment

      // remember start position for error reporting
      const std::size_t error_line = cursor_.line();
      const std::size_t error_col = cursor_.column();

      cursor_.next();
      cursor_.next();  // skip /*

      while (!cursor_.eof()) {
        if (cursor_.peek() == '*' && cursor_.peek_at(1) == '/') {
          cursor_.next();
          cursor_.next();
          break;
        }
        cursor_.next();
      }

      // check if we reached eof without finding closing */
      if (cursor_.eof() &&
          !(cursor_.peek() == '*' && cursor_.peek_at(1) == '/')) {
        return Result<void>(diagnostic::create_err(Error::create(
            error_line, error_col, 2,
            diagnostic::DiagnosticId::kUnterminatedBlockComment)));
      }

      continue;
    }

    break;
  }

  return Result<void>(diagnostic::create_ok());
}

Lexer::Result<void> Lexer::skip_trivia() {
  while (!cursor_.eof()) {
    const std::size_t old_position = cursor_.position();

    if (!should_include_whitespace()) {
      skip_whitespace();
    }

    if (!should_include_normal_comments() ||
        !should_include_documentation_comments()) {
      const auto result = skip_comments();
      if (result.is_err()) {
        // propagate error
        return result;
      }
    }

    if (cursor_.position() == old_position) {
      break;
    }
  }

  return Result<void>(diagnostic::create_ok());
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
