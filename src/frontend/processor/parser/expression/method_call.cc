// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_method_call_expr(
    NodeId obj,
    PayloadId<ast::PathExpressionPayload> method) {
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

  const PayloadId<ast::MethodCallExpressionPayload> method_id =
      context_->alloc_payload(ast::MethodCallExpressionPayload{
          .obj = obj,
          .method = method,
          .args_range = std::move(args_r).unwrap(),
      });

  if (check(base::TokenKind::kArrow)) {
    const NodeId method_node_id = context_->alloc(Node{
        .payload_id = method_id.id,
        .kind = ast::NodeKind::kMethodCallExpression,
    });
    return parse_await_expr(method_node_id);
  } else {
    return ok(context_->alloc(Node{
        .payload_id = method_id.id,
        .kind = ast::NodeKind::kMethodCallExpression,
    }));
  }
}

}  // namespace parser
