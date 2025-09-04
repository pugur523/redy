// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::BreakExpressionPayload>;

Parser::Result<R> Parser::parse_break_expr() {
  auto break_r = consume(base::TokenKind::kBreak, false);
  if (break_r.is_err()) {
    return err<R>(std::move(break_r));
  }

  NodeId expr_id = ast::kInvalidNodeId;
  if (peek().kind() != base::TokenKind::kNewline &&
      peek().kind() != base::TokenKind::kSemicolon) {
    auto expr_r = parse_expression();
    if (expr_r.is_err()) {
      return err<R>(std::move(expr_r));
    }
    expr_id = std::move(expr_r).unwrap();
  }

  return ok(context_->alloc_payload(
      ast::BreakExpressionPayload{.expression = expr_id}));
}

}  // namespace parser
