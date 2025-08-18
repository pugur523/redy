// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/operator.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

// precedence climbing for binary expressions
// lower precedence value means higher priority
Parser::Result<ast::NodeId> Parser::parse_binary_expression(
    base::OperatorPrecedence min_precedence) {
  // parse left operand (unary expression)
  auto left_r = parse_unary_expression();
  if (left_r.is_err()) {
    return left_r;
  }
  ast::NodeId left_id = std::move(left_r).unwrap();

  while (!eof()) {
    const base::TokenKind kind = peek().kind();
    if (!base::token_kind_is_binary_operator(kind)) {
      break;
    }

    const base::BinaryOperator op = base::token_kind_to_binary_op(kind);
    const base::OperatorPrecedence current_precedence =
        base::binary_op_to_precedence(op);

    // check if current operator has sufficient precedence
    if (current_precedence > min_precedence) {
      // precedence too low, let higher level handle it
      break;
    }

    next_non_whitespace();

    // determine precedence for right operand based on associativity
    base::OperatorPrecedence right_min_precedence;
    if (base::operator_precedence_to_associativity(current_precedence) ==
        base::OperatorAssociativity::kRightToLeft) {
      right_min_precedence = current_precedence;
    } else {
      right_min_precedence = static_cast<base::OperatorPrecedence>(
          static_cast<uint8_t>(current_precedence) - 1);
    }

    // recursive approach
    auto right_r = parse_binary_expression(right_min_precedence);
    if (right_r.is_err()) {
      return right_r;
    }
    const ast::NodeId right_id = std::move(right_r).unwrap();

    left_id = context_->alloc(ast::BinaryOperatorExpressionNode{
        .op = op,
        .lhs = left_id,
        .rhs = right_id,
    });
  }

  return ok(left_id);
}

}  // namespace parser
