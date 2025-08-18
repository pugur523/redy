// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_grouped_expression() {
  auto left_r = consume(base::TokenKind::kLeftParen, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  auto expr_r = parse_expression();
  if (expr_r.is_err()) {
    return expr_r;
  }

  auto right_r = consume(base::TokenKind::kRightParen, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->alloc(ast::GroupedExpressionNode{
      .expression = std::move(expr_r).unwrap(),
  }));
}

}  // namespace parser
