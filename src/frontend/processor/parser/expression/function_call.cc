// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_function_call_expr(NodeId callee) {
  auto left_r = consume(base::TokenKind::kLeftParen, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r));
  }

  auto args_r = parse_expression_sequence();
  if (args_r.is_err()) {
    return err<NodeId>(std::move(args_r));
  }

  auto right_r = consume(base::TokenKind::kRightParen, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r));
  }

  const PayloadId<ast::FunctionCallExpressionPayload> function_id =
      context_->alloc_payload(ast::FunctionCallExpressionPayload{
          .callee = callee,
          .args_range = std::move(args_r).unwrap(),
      });

  if (check(base::TokenKind::kArrow)) {
    const NodeId func_node_id = context_->alloc(Node{
        .payload_id = function_id.id,
        .kind = ast::NodeKind::kFunctionCallExpression,
    });
    return parse_await_expr(func_node_id);
  } else {
    return ok(context_->alloc(Node{
        .payload_id = function_id.id,
        .kind = ast::NodeKind::kFunctionCallExpression,
    }));
  }
}

}  // namespace parser
