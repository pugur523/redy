// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_function_macro_call_expression(
    NodeId callee) {
  auto hash_r = consume(base::TokenKind::kHash, true);
  if (hash_r.is_err()) {
    return err<NodeId>(std::move(hash_r).unwrap_err());
  }

  auto left_r = consume(base::TokenKind::kLeftParen, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  auto args_r = parse_expression_sequence();
  if (args_r.is_err()) {
    return err<NodeId>(std::move(args_r).unwrap_err());
  }

  auto right_r = consume(base::TokenKind::kRightParen, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  const NodeId macro_id = context_->alloc(ast::FunctionMacroCallExpressionNode{
      .macro_callee = callee,
      .args_range = std::move(args_r).unwrap(),
  });

  if (peek().kind() == base::TokenKind::kArrow) {
    return parse_await_expression(macro_id);
  } else {
    return ok(macro_id);
  }
}

}  // namespace parser
