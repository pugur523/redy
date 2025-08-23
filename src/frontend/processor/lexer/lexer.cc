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

Lexer::InitResult Lexer::init(unicode::Utf8FileManager* file_manager,
                              unicode::Utf8FileId file_id,
                              Mode mode) {
  DCHECK_EQ(status_, Status::kNotInitialized);
  mode_ = mode;

  std::size_t result = stream_.init(file_manager, file_id);

  if (result == 0) {
    status_ = Status::kReadyToTokenize;
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
  DCHECK_EQ(status_, Status::kReadyToTokenize);
  std::vector<Token> tokens;
  std::vector<Error> errors;

  tokens.reserve(stream_.file().line_count() * kPredictedTokensCountPerLine);

  while (true) {
    Result<Token> result = tokenize_next();
    if (result.is_ok()) [[likely]] {
      Token token{std::move(result).unwrap()};
      const TokenKind kind = token.kind();
      tokens.push_back(std::move(token));

      if (kind == TokenKind::kEof) {
        break;
      }
    } else {
      errors.push_back(result.unwrap_err());
      if (strict) {
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
  DCHECK_NE(status_, Status::kNotInitialized);
  DCHECK_NE(status_, Status::kTokenizeCompleted);
  auto r = skip_trivia();
  if (r.is_err()) {
    status_ = Status::kErrorOccured;
    return Result<Lexer::Token>(
        diagnostic::create_err(std::move(r).unwrap_err()));
  }

  if (stream_.eof()) {
    status_ = Status::kTokenizeCompleted;
    return Result<Token>(diagnostic::create_ok(
        Token(TokenKind::kEof, stream_.line(), stream_.column(), 0)));
  }

  const char32_t current_codepoint = stream_.peek();
  const std::size_t line = stream_.line();
  const std::size_t col = stream_.column();
  const std::size_t start = stream_.position();

  if (unicode::is_eof(current_codepoint)) {
    status_ = Status::kErrorOccured;
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
  while (!stream_.eof() && !unicode::is_unicode_newline(stream_.peek()) &&
         unicode::is_unicode_whitespace(stream_.peek())) {
    stream_.next();
  }
}

Lexer::Result<void> Lexer::skip_comments() {
  while (!stream_.eof()) {
    if (stream_.peek() == '/' && stream_.peek_at(1) == '/') {
      const char32_t third_cp = stream_.peek_at(2);
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
      stream_.next();
      stream_.next();  // skip //
      while (!stream_.eof() && !unicode::is_unicode_newline(stream_.peek())) {
        stream_.next();
      }
      continue;
    }

    if (stream_.peek() == '/' && stream_.peek_at(1) == '*') {
      if (should_include_normal_comments()) {
        break;
      }
      // skip block comment

      // remember start position for error reporting
      const std::size_t error_line = stream_.line();
      const std::size_t error_col = stream_.column();

      stream_.next();
      stream_.next();  // skip /*

      while (!stream_.eof()) {
        if (stream_.peek() == '*' && stream_.peek_at(1) == '/') {
          stream_.next();
          stream_.next();
          break;
        }
        stream_.next();
      }

      // check if we reached eof without finding closing */
      if (stream_.eof() &&
          !(stream_.peek() == '*' && stream_.peek_at(1) == '/')) {
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
  while (!stream_.eof()) {
    const std::size_t old_position = stream_.position();

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

    if (stream_.position() == old_position) {
      break;
    }
  }

  return Result<void>(diagnostic::create_ok());
}

Lexer::Result<Lexer::Token> Lexer::identifier_or_keyword() {
  const std::size_t start = stream_.position();
  const std::size_t line = stream_.line();
  const std::size_t col = stream_.column();

  while (unicode::is_xid_continue(stream_.peek())) {
    stream_.next();
  }

  TokenKind kind = base::lookup_id_or_keyword(
      stream_.file().content_u8(), start, stream_.position() - start);
  return create_token(kind, start, line, col);
}

}  // namespace lexer
