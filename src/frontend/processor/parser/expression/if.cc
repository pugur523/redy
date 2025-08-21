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

Parser::Result<ast::NodeId> Parser::parse_if_expression() {
  // parse first branch
  auto if_r = consume(base::TokenKind::kIf, true);
  if (if_r.is_err()) {
    return err<NodeId>(std::move(if_r).unwrap_err());
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

  const PayloadId first_id = context_->alloc(ast::IfBranchPayload{
      .condition = cond_id,
      .block = block_id,
  });

  uint32_t branches_count = 1;
  while (!eof()) {
    if (peek().kind() != base::TokenKind::kElse) {
      break;
    }
    // consume else
    next_non_whitespace();

    bool is_else_if;
    NodeId cond_id = ast::kInvalidNodeId;
    if (peek().kind() == base::TokenKind::kIf) {
      // else if
      is_else_if = true;

      // consume if
      next_non_whitespace();
      auto cond_r = parse_expression();
      if (cond_r.is_err()) {
        return cond_r;
      }
      cond_id = std::move(cond_r).unwrap();
    } else {
      is_else_if = false;
      cond_id = ast::kInvalidNodeId;
    }

    auto block_r = parse_block_expression();
    if (block_r.is_err()) {
      return block_r;
    }
    const NodeId block_id = std::move(block_r).unwrap();

    context_->alloc(ast::IfBranchPayload{
        .condition = cond_id,
        .block = block_id,
    });

    ++branches_count;

    if (!is_else_if) {
      // last else
      break;
    }
  }

  return ok(context_->create(
      ast::NodeKind::kIfExpression,
      ast::IfExpressionPayload{
          .branches_range = {.begin = first_id, .size = branches_count},
      }));
}

}  // namespace parser
