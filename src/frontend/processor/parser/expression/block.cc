// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_block_expression() {
  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  NodeId first = ast::kInvalidNodeId;
  uint32_t body_statement_count = 0;

  while (!eof()) {
    auto body_statement_r = parse_statement();
    if (body_statement_r.is_err()) {
      return body_statement_r;
    }

    if (body_statement_count == 0) {
      first = std::move(body_statement_r).unwrap();
    }
    ++body_statement_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->create(
      ast::NodeKind::kBlockExpression,
      ast::BlockExpressionPayload{
          .body_nodes_range = {.begin = first, .size = body_statement_count}}));
}

}  // namespace parser
