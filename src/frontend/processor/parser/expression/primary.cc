// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_primary_expr() {
  const base::TokenKind kind = peek().kind();

  if (base::token_kind_is_literal(kind)) {
    return wrap_to_node(ast::NodeKind::kLiteralExpression,
                        parse_literal_expr());
  }

  switch (kind) {
    case base::TokenKind::kIdentifier: return parse_postfix_expr();
    case base::TokenKind::kBreak:
      return wrap_to_node(ast::NodeKind::kBreakExpression, parse_break_expr());
    case base::TokenKind::kContinue:
      return wrap_to_node(ast::NodeKind::kContinueExpression,
                          parse_continue_expr());
    case base::TokenKind::kReturn:
      return wrap_to_node(ast::NodeKind::kReturnExpression,
                          parse_return_expr());
    case base::TokenKind::kLeftParen:
      return wrap_to_node(ast::NodeKind::kGroupedExpression,
                          parse_grouped_expr());
    case base::TokenKind::kLeftBrace:
      return wrap_to_node(ast::NodeKind::kBlockExpression, parse_block_expr());
    case base::TokenKind::kIf:
      return wrap_to_node(ast::NodeKind::kIfExpression, parse_if_expr());
    case base::TokenKind::kLoop:
      return wrap_to_node(ast::NodeKind::kLoopExpression, parse_loop_expr());
    case base::TokenKind::kWhile:
      return wrap_to_node(ast::NodeKind::kWhileExpression, parse_while_expr());
    case base::TokenKind::kFor:
      return wrap_to_node(ast::NodeKind::kForExpression, parse_for_expr());
    case base::TokenKind::kMatch:
      return wrap_to_node(ast::NodeKind::kMatchExpression, parse_match_expr());
    case base::TokenKind::kLeftBracket:
      return wrap_to_node(ast::NodeKind::kClosureExpression,
                          parse_closure_expr());
    default:
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kUnexpectedToken)
                  .label(stream_->file_id(), peek().range(),
                         i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                         diagnostic::LabelMarkerType::kEmphasis,
                         {translator_->translate(
                             base::token_kind_to_tr_key(kind))}))
              .build());
  }
}

}  // namespace parser
