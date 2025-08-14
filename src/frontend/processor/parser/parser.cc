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
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/diagnostic/data/severity.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"
#include "unicode/utf8/file_manager.h"

namespace parser {

void Parser::init(base::TokenStream* stream,
                  unicode::Utf8FileId file_id,
                  const i18n::Translator& translator) {
  stream_ = stream;
  DCHECK(stream_);
  file_id_ = file_id;

  translator_ = &translator;
  DCHECK(translator_);

  init_context();

  status_ = Status::kReadyToParse;
  return;
}

void Parser::init_context() {
  context_ = ast::Context::create();
  DCHECK(context_);

  // TODO: hotpath heuristic allocations
  context_->arena<ast::FunctionDeclarationNode>().reserve(32);
  context_->arena<ast::AssignStatementNode>().reserve(128);
}

Parser::Results Parser::parse_all(bool strict) {
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
    next();
  }

  if (errors_.empty()) {
    status_ = Status::kParseCompleted;
    return ok();
  } else {
    status_ = Status::kErrorOccured;
    return Results(diagnostic::create_err(std::move(errors_)));
  }
}

Parser::Result<void> Parser::parse_root() {
  using Kind = base::TokenKind;

  const Kind current_kind = peek().kind();

  if (eof() || current_kind == Kind::kEof) {
    return ok<void>();
  } else if (base::token_kind_is_declaration_keyword(current_kind) ||
             base::token_kind_is_attribute_keyword(current_kind)) [[likely]] {
    return parse_declaration();
  } else if (current_kind == Kind::kIdentifier) {
    // global variable assignment
    const Kind next_kind = peek_at(1).kind();
    switch (next_kind) {
      case Kind::kColonEqual:
      case Kind::kColon: return parse_statement();
      default:
        return err<void>(
            std::move(
                Eb(diagnostic::Severity::kError,
                   diagnostic::DiagnosticId::kUnexpectedToken)
                    .label(
                        file_id_, peek().range(),
                        i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                        diagnostic::LabelMarkerType::kLine,
                        {translator_->translate(
                            base::token_kind_to_tr_key(current_kind))}))
                .build());
    }
  } else {
    err<void>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagnosticId::kUnexpectedToken)
                .label(file_id_, peek().range(),
                       i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                       diagnostic::LabelMarkerType::kLine,
                       {translator_->translate(
                           base::token_kind_to_tr_key(current_kind))}))
            .build());
  }
  return ok<void>();
}

void Parser::append_errors(std::vector<De>&& new_errors) {
  errors_.insert(errors_.end(), std::make_move_iterator(new_errors.begin()),
                 std::make_move_iterator(new_errors.end()));
}

Parser::Result<const base::Token*> Parser::consume(base::TokenKind expected) {
  const auto& token = stream_->peek();
  if (check(expected)) [[likely]] {
    next();
    return ok<const base::Token*>(&token);
  } else {
    return Result<const base::Token*>(diagnostic::create_err(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagId::kExpectedButFound)
                .label(file_id_, token.range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis,
                       {translator_->translate(
                            base::token_kind_to_tr_key(expected)),
                        translator_->translate(
                            base::token_kind_to_tr_key(token.kind()))}))
            .build()));
  }
}

void Parser::synchronize() {
  if (stream_->eof()) {
    return;
  }
  stream_->next();  // consume invalid token
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
