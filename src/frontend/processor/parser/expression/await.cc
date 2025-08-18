// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_await_expression(NodeId callee) {
  auto arrow_r = consume(base::TokenKind::kArrow, true);
  if (arrow_r.is_err()) {
    return err<NodeId>(std::move(arrow_r).unwrap_err());
  }

  auto await_r = consume(base::TokenKind::kAwait, true);
  if (await_r.is_err()) {
    return err<NodeId>(std::move(await_r).unwrap_err());
  }

  return ok(context_->alloc(ast::AwaitExpressionNode{
      .expression = callee,
  }));
}

}  // namespace parser
