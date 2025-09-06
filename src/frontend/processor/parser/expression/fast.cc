// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::FastExpressionPayload>;

Parser::Result<R> Parser::parse_fast_expr() {
  auto fast_r = consume(base::TokenKind::kFast, true);
  if (fast_r.is_err()) {
    return err<R>(std::move(fast_r));
  }

  auto block_r = parse_block_expr();
  if (block_r.is_err()) {
    return err<R>(std::move(block_r));
  }

  return ok(context_->alloc_payload(ast::FastExpressionPayload{
      .block = std::move(block_r).unwrap(),
  }));
}

}  // namespace parser
