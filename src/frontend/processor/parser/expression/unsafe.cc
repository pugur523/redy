// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_unsafe_expression() {
  auto unsafe_r = consume(base::TokenKind::kUnsafe, true);
  if (unsafe_r.is_err()) {
    return err<NodeId>(std::move(unsafe_r).unwrap_err());
  }

  auto block_r = parse_block_expression();
  if (block_r.is_err()) {
    return block_r;
  }

  const PayloadId payload_id = context_->alloc(ast::UnsafeExpressionPayload{
      .block = std::move(block_r).unwrap(),
  });

  return ok(context_->alloc(ast::Node{
      .kind = ast::NodeKind::kUnsafeExpression,
      .payload_id = payload_id,
  }));
}

}  // namespace parser
