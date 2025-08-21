// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_while_expression() {
  auto while_r = consume(base::TokenKind::kWhile, true);
  if (while_r.is_err()) {
    return err<NodeId>(std::move(while_r).unwrap_err());
  }

  auto cond_r = parse_expression();
  if (cond_r.is_err()) {
    return cond_r;
  }
  const NodeId cond_id = std::move(cond_r).unwrap();

  auto block_r = parse_block_expression();
  if (block_r.is_err()) {
    return block_r;
  }
  const NodeId block_id = std::move(block_r).unwrap();

  const PayloadId payload_id = context_->alloc(ast::WhileExpressionPayload{
      .condition = cond_id,
      .body = block_id,
  });

  return ok(context_->alloc(ast::Node{
      .kind = ast::NodeKind::kWhileExpression,
      .payload_id = payload_id,
  }));
}

}  // namespace parser
