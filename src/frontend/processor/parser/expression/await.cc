// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_await_expr(NodeId callee) {
  DCHECK(check(base::TokenKind::kArrow));
  // consume ->
  next_non_whitespace();
  // auto arrow_r = consume(base::TokenKind::kArrow, true);
  // if (arrow_r.is_err()) {
  //   return err<NodeId>(std::move(arrow_r));
  // }

  auto await_r = consume(base::TokenKind::kAwait, true);
  if (await_r.is_err()) {
    return err<NodeId>(std::move(await_r));
  }

  return ok(context_->alloc_node(ast::NodeKind::kAwaitExpression,
                                 ast::AwaitExpressionPayload{
                                     .callee_expression = callee,
                                 }));
}

}  // namespace parser
