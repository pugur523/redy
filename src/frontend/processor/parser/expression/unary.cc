// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/operator/unary_operator.h"
#include "frontend/base/token/token.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_unary_expr() {
  uint32_t pre_op_count;
  for (pre_op_count = 0; pre_op_count < stream_->size(); ++pre_op_count) {
    if (!base::token_kind_is_unary_operator(peek().kind())) {
      break;
    }
    next_non_whitespace();
  }

  // parse the primary expression (the operand)
  auto operand_r = parse_primary_expr();
  if (operand_r.is_err()) {
    return operand_r;
  } else if (pre_op_count == 0) {
    return operand_r;
  }
  NodeId operand_id = std::move(operand_r).unwrap();

  // rewind to first unary operator position
  stream_->rewind(stream_->position() - pre_op_count);

  // apply prefix operators in reverse order (right-to-left associativity)
  // i.e., `++--x` becomes `++(--x)`
  for (uint32_t i = pre_op_count; i > 0; --i) {
    const base::UnaryOperator op = base::token_kind_to_unary_op(
        peek_at(i).kind(), base::IncrementPosition::kPrefix);
    operand_id = context_->alloc_node(ast::NodeKind::kUnaryExpression,
                                      ast::UnaryExpressionPayload{
                                          .op = op,
                                          .operand = operand_id,
                                      });
  }

  // handle postfix unary operators (left-to-right associativity)
  while (base::token_kind_is_unary_operator(peek().kind())) {
    const base::Token& postfix_token = peek();

    const base::UnaryOperator postfix_op = base::token_kind_to_unary_op(
        postfix_token.kind(), base::IncrementPosition::kPostfix);

    // check if this operator can be used as postfix
    if (!base::is_postfix_operator(postfix_op)) {
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagId::kCannotBePostfixOperator)
                  .label(stream_->file_id(), postfix_token.range(),
                         i18n::TranslationKey::
                             kDiagnosticParserCannotBePostfixOperator,
                         diagnostic::LabelMarkerType::kEmphasis,
                         {translator_->translate(base::token_kind_to_tr_key(
                             postfix_token.kind()))}))
              .build());
    }

    next_non_whitespace();
    operand_id = context_->alloc_node(ast::NodeKind::kUnaryExpression,
                                      ast::UnaryExpressionPayload{
                                          .op = postfix_op,
                                          .operand = operand_id,
                                      });
  }

  return ok(operand_id);
}

}  // namespace parser
