// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_return_expression() {
  auto return_r = consume(base::TokenKind::kReturn, false);
  if (return_r.is_err()) {
    return err<NodeId>(std::move(return_r).unwrap_err());
  }

  NodeId expr_id = ast::kInvalidNodeId;
  if (peek().kind() != base::TokenKind::kNewline &&
      peek().kind() != base::TokenKind::kSemicolon) {
    auto expr_r = parse_primary_expression();
    if (expr_r.is_err()) {
      return expr_r;
    }
    expr_id = std::move(expr_r).unwrap();
  }

  return ok(context_->alloc(ast::ReturnExpressionNode{
      .expression = expr_id,
  }));
}

}  // namespace parser
