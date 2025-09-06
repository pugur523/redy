// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::GroupedExpressionPayload>;

Parser::Result<R> Parser::parse_grouped_expr() {
  auto left_r = consume(base::TokenKind::kLeftParen, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  auto expr_r = parse_expression();
  if (expr_r.is_err()) {
    return err<R>(std::move(expr_r));
  }

  auto right_r = consume(base::TokenKind::kRightParen, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::GroupedExpressionPayload{
      .expression = std::move(expr_r).unwrap(),
  }));
}

}  // namespace parser
