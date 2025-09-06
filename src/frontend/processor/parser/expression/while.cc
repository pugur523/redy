// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::WhileExpressionPayload>;

Parser::Result<R> Parser::parse_while_expr() {
  auto while_r = consume(base::TokenKind::kWhile, true);
  if (while_r.is_err()) {
    return err<R>(std::move(while_r));
  }

  auto cond_r = parse_expression();
  if (cond_r.is_err()) {
    return err<R>(std::move(cond_r));
  }
  const NodeId cond_id = std::move(cond_r).unwrap();

  auto block_r = parse_block_expr();
  if (block_r.is_err()) {
    return err<R>(std::move(block_r));
  }

  return ok(context_->alloc_payload(ast::WhileExpressionPayload{
      .condition = cond_id,
      .body = std::move(block_r).unwrap(),
  }));
}

}  // namespace parser
