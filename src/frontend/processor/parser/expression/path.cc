// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_path_expression() {
  ast::NodeId first_id = ast::kInvalidNodeId;
  uint32_t parts_count = 0;

  base::TokenKind kind = peek().kind();
  while (!eof() && (kind == base::TokenKind::kIdentifier ||
                    kind == base::TokenKind::kColonColon ||
                    kind == base::TokenKind::kNewline)) {
    switch (kind) {
      case base::TokenKind::kIdentifier: {
        if (parts_count == 0) {
          first_id = context_->alloc(
              ast::IdentifierNode{peek().lexeme(stream_->file())});
        } else {
          context_->alloc(ast::IdentifierNode{peek().lexeme(stream_->file())});
        }
        ++parts_count;
      }
      case base::TokenKind::kColonColon: {
        const base::Token& next_token = peek_at(1);
        if (next_token.kind() != base::TokenKind::kIdentifier) {
          return err<ast::NodeId>(
              std::move(
                  Eb(diagnostic::Severity::kError,
                     diagnostic::DiagId::kExpectedButFound)
                      .label(
                          stream_->file_id(), next_token.range(),
                          i18n::TranslationKey::
                              kDiagnosticParserExpectedButFound,
                          diagnostic::LabelMarkerType::kEmphasis,
                          {translator_->translate(
                               i18n::TranslationKey::kTermTokenKindIdentifier),
                           translator_->translate(
                               base::token_kind_to_tr_key(next_token.kind()))}))
                  .build());
        }
        break;
      }
      case base::TokenKind::kNewline: break;
      default: break;
    }
    kind = next_non_whitespace().kind();
  }

  return ok(context_->alloc(ast::PathExpressionNode{
      .path_parts_range = {first_id, parts_count},
  }));
}

}  // namespace parser
