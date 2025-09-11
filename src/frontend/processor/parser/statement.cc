// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/keyword/declaration_keyword.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_statement() {
  using Kind = base::TokenKind;
  const Kind current_kind = peek().kind();

  if (base::token_kind_is_declaration_keyword(current_kind) ||
      base::token_kind_is_attribute_keyword(current_kind)) [[likely]] {
    auto result = parse_decl_stmt();
    if (result.is_err()) {
      return err<NodeId>(std::move(result));
    }
    return ok<NodeId>(std::move(result).unwrap());
  }

  switch (current_kind) {
    case base::TokenKind::kIdentifier: {
      const Kind next_kind = peek_at(1).kind();
      switch (next_kind) {
        case Kind::kColonEqual:
        case Kind::kEqual:
        case Kind::kColon: {
          // pass empty attribute
          auto result = parse_assign_stmt({});
          if (result.is_err()) {
            return err<NodeId>(std::move(result));
          } else {
            return ok<NodeId>(context_->alloc(Node{
                .payload_id = std::move(result).unwrap().id,
                .kind = ast::NodeKind::kAssignStatement,
            }));
          }
        }
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
    case base::TokenKind::kHash: {
      auto result = parse_attribute_stmt();
      if (result.is_err()) {
        return err<NodeId>(std::move(result));
      } else {
        return ok<NodeId>(context_->alloc(Node{
            .payload_id = std::move(result).unwrap().id,
            .kind = ast::NodeKind::kAttributeStatement,
        }));
      }
    }
    case base::TokenKind::kUse: {
      auto result = parse_use_stmt();
      if (result.is_err()) {
        return err<NodeId>(std::move(result));
      } else {
        return ok<NodeId>(context_->alloc(Node{
            .payload_id = std::move(result).unwrap().id,
            .kind = ast::NodeKind::kUseStatement,
        }));
      }
    }
    default: {
      // TODO: replace this with `return parse_expression()` and remove
      // `parse_expression_stmt()`
      auto result = parse_expression_stmt();
      if (result.is_err()) {
        return err<NodeId>(std::move(result));
      } else {
        return ok<NodeId>(context_->alloc(Node{
            .payload_id = std::move(result).unwrap().id,
            .kind = ast::NodeKind::kExpressionStatement,
        }));
      }
    }
  }
}

}  // namespace parser
