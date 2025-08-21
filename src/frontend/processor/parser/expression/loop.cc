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

Parser::Result<ast::NodeId> Parser::parse_loop_expression() {
  auto loop_r = consume(base::TokenKind::kLoop, true);
  if (loop_r.is_err()) {
    return err<NodeId>(std::move(loop_r).unwrap_err());
  }

  auto block_r = parse_block_expression();
  if (block_r.is_err()) {
    return block_r;
  }
  const NodeId block_id = std::move(block_r).unwrap();

  return ok(context_->create(ast::NodeKind::kLoopExpression,
                             ast::LoopExpressionPayload{
                                 .body = block_id,
                             }));
}

}  // namespace parser
