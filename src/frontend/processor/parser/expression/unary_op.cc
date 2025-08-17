// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/operator/unary_operator.h"
#include "frontend/base/token/token.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_unary_operator_expression() {
  const base::Token& current = peek();
  if (base::token_kind_is_unary_operator(current.kind())) {
    // handle prefix unary operators
    base::UnaryOperator op = base::token_kind_to_unary_op(
        current.kind(), base::IncrementPosition::kPrefix);
    next();
    // recursively parse the operand (handles nested prefix operators like
    // ++++x)
    auto operand_r = parse_unary_operator_expression();
    if (operand_r.is_err()) {
      return err<ast::NodeId>(std::move(operand_r).unwrap_err());
    }

    ast::NodeId operand_id = std::move(operand_r).unwrap();
    ast::NodeId prefix_expr_id =
        context_->alloc(ast::UnaryOperatorExpressionNode{
            .op = op,
            .operand = operand_id,
        });

    return ok<ast::NodeId>(prefix_expr_id);
  }

  // no prefix operator found, parse primary expression
  auto operand_r = parse_primary_expression();
  if (operand_r.is_err()) {
    return err<ast::NodeId>(std::move(operand_r).unwrap_err());
  }

  ast::NodeId operand_id = std::move(operand_r).unwrap();

  // handle postfix unary operators
  while (base::token_kind_is_unary_operator(peek().kind())) {
    const base::Token& postfix_token = peek();

    base::UnaryOperator postfix_op = base::token_kind_to_unary_op(
        postfix_token.kind(), base::IncrementPosition::kPostfix);
    next();
    if (!base::is_postfix_operator(postfix_op)) {
      return err<ast::NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagId::kExpectedButFound)
                  .label(stream_->file_id(), postfix_token.range(),
                         i18n::TranslationKey::
                             kDiagnosticParserCannotBePostfixOperator,
                         diagnostic::LabelMarkerType::kEmphasis,
                         {translator_->translate(base::token_kind_to_tr_key(
                             postfix_token.kind()))}))
              .build());
    }

    operand_id = context_->alloc(ast::UnaryOperatorExpressionNode{
        .op = postfix_op,
        .operand = operand_id,
    });
  }

  return ok<ast::NodeId>(operand_id);
}

}  // namespace parser
