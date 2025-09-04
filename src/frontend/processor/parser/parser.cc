// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/parser/parser.h"

#include <iterator>
#include <utility>

#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/keyword/control_flow_keyword.h"
#include "frontend/base/keyword/declaration_keyword.h"
#include "frontend/base/keyword/keyword.h"
#include "frontend/base/keyword/modifier_keyword.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/diagnostic/data/severity.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"
#include "unicode/utf8/file_manager.h"

namespace parser {

void Parser::init(base::TokenStream* stream,
                  const i18n::Translator& translator) {
  DCHECK_EQ(status_, Status::kNotInitialized);
  stream_ = stream;
  DCHECK(stream_);

  translator_ = &translator;
  DCHECK(translator_);

  init_context();

  status_ = Status::kReadyToParse;
  return;
}

void Parser::init_context() {
  DCHECK_EQ(status_, Status::kNotInitialized);
  context_ = ast::Context::create();
  DCHECK(context_);

  // TODO: hot path heuristic allocations
  context_->arena<ast::Node>().reserve(512);

  context_->arena<ast::FunctionDeclarationPayload>().reserve(32);
  context_->arena<ast::AssignStatementPayload>().reserve(128);
  context_->arena<ast::TypeReferencePayload>().reserve(64);
  context_->arena<ast::ParameterPayload>().reserve(256);
  context_->arena<ast::IdentifierPayload>().reserve(256);
}

Parser::Results Parser::parse_all(bool strict) {
  DCHECK_EQ(status_, Status::kReadyToParse);
  while (!eof()) {
    parse_root();
    if (!errors_.empty()) [[unlikely]] {
      if (strict) {
        break;
      } else {
        synchronize();
        continue;
      }
    }
  }

  if (errors_.empty()) {
    status_ = Status::kParseCompleted;
    return ok();
  } else {
    status_ = Status::kErrorOccured;
    return Results(diagnostic::create_err(std::move(errors_)));
  }
}

void Parser::parse_root() {
  const base::TokenKind current_kind = peek().kind();

  // parses declaration or statement
  if (eof() || current_kind == base::TokenKind::kEof) {
    return;
  } else if (current_kind == base::TokenKind::kBlockComment ||
             current_kind == base::TokenKind::kInlineComment) {
    // TODO: support document gen mode
    return;
  } else if (base::token_kind_is_declaration_keyword(current_kind) ||
             base::token_kind_is_attribute_keyword(current_kind)) [[likely]] {
    auto result = parse_decl_stmt();
    if (result.is_err()) {
      errors_.emplace_back(std::move(result).unwrap_err());
    }
    return;
  } else {
    auto result = parse_statement();
    if (result.is_err()) {
      errors_.emplace_back(std::move(result).unwrap_err());
    }
    return;
  }
}

void Parser::append_errors(std::vector<De>&& new_errors) {
  errors_.insert(errors_.end(), std::make_move_iterator(new_errors.begin()),
                 std::make_move_iterator(new_errors.end()));
}

Parser::Result<const base::Token*> Parser::consume(base::TokenKind expected,
                                                   bool skip_whitespaces) {
  const auto& token = stream_->peek();
  if (check(expected)) [[likely]] {
    if (skip_whitespaces) {
      next_non_whitespace();
    } else {
      next();
    }
    return ok<const base::Token*>(&token);
  } else {
    return err<const base::Token*>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagId::kExpectedButFound)
                .label(stream_->file_id(), token.range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis,
                       {translator_->translate(
                            base::token_kind_to_tr_key(expected)),
                        translator_->translate(
                            base::token_kind_to_tr_key(token.kind()))}))
            .build());
  }
}

void Parser::synchronize() {
  if (stream_->eof()) [[unlikely]] {
    return;
  }

  // consume invalid token
  stream_->next();
  while (!stream_->eof()) {
    if (is_sync_point(stream_->peek().kind())) {
      return;
    }
    stream_->next();
  }
}

// static
bool Parser::is_sync_point(base::TokenKind kind) {
  if (base::token_kind_is_control_flow_keyword(kind) ||
      base::token_kind_is_declaration_keyword(kind) ||
      base::token_kind_is_attribute_keyword(kind)) {
    return true;
  }
  switch (kind) {
    // delimiters
    case base::TokenKind::kSemicolon:
    case base::TokenKind::kRightBrace:
    case base::TokenKind::kNewline:
    case base::TokenKind::kEof: return true;

    default: return false;
  }
}

}  // namespace parser
