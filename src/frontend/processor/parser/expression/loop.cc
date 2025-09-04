// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::LoopExpressionPayload>;

Parser::Result<R> Parser::parse_loop_expr() {
  auto loop_r = consume(base::TokenKind::kLoop, true);
  if (loop_r.is_err()) {
    return err<R>(std::move(loop_r));
  }

  auto block_r = parse_block_expr();
  if (block_r.is_err()) {
    return err<R>(std::move(block_r));
  }

  return ok(context_->alloc_payload(ast::LoopExpressionPayload{
      .body = std::move(block_r).unwrap(),
  }));
}

}  // namespace parser
