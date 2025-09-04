// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::BlockExpressionPayload>;

Parser::Result<R> Parser::parse_block_expr() {
  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  NodeId first_id = ast::kInvalidNodeId;
  uint32_t body_statement_count = 0;

  while (!eof() && !check(base::TokenKind::kRightBrace)) {
    auto body_statement_r = parse_statement();
    if (body_statement_r.is_err()) {
      return err<R>(std::move(body_statement_r));
    }

    if (body_statement_count == 0) {
      first_id = std::move(body_statement_r).unwrap();
    }
    ++body_statement_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::BlockExpressionPayload{
      .body_nodes_range = {.begin = first_id, .size = body_statement_count}}));
}

}  // namespace parser
