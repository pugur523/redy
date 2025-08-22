// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_statement() {
  using Kind = base::TokenKind;
  const Kind current_kind = peek().kind();

  switch (current_kind) {
    case base::TokenKind::kIdentifier: {
      const Kind next_kind = peek_at(1).kind();
      switch (next_kind) {
        case Kind::kColonEqual:
        case Kind::kEqual:
        case Kind::kColon:
          return parse_assign_statement(ast::kInvalidPayloadId);
        default:
          return err<NodeId>(
              std::move(
                  Eb(diagnostic::Severity::kError,
                     diagnostic::DiagnosticId::kUnexpectedToken)
                      .label(stream_->file_id(), peek().range(),
                             i18n::TranslationKey::
                                 kDiagnosticParserUnexpectedToken,
                             diagnostic::LabelMarkerType::kLine,
                             {translator_->translate(
                                 base::token_kind_to_tr_key(current_kind))}))
                  .build());
      }
    }
    case base::TokenKind::kHash: return parse_attribute_statement();
    default: return parse_expression_statement();
  }
}

}  // namespace parser
