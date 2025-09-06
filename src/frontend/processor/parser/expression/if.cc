// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::IfExpressionPayload>;

Parser::Result<R> Parser::parse_if_expr() {
  // parse first branch
  auto if_r = consume(base::TokenKind::kIf, true);
  if (if_r.is_err()) {
    return err<R>(std::move(if_r));
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

  const PayloadId<ast::IfBranchPayload> first_id =
      context_->alloc_payload(ast::IfBranchPayload{
          .condition = cond_id,
          .block = std::move(block_r).unwrap(),
      });

  uint32_t branches_count = 1;
  while (!eof() && check(base::TokenKind::kElse)) {
    // consume else
    next_non_whitespace();

    bool is_else_if;
    NodeId cond_id = ast::kInvalidNodeId;
    if (check(base::TokenKind::kIf)) {
      // else if
      is_else_if = true;

      // consume if
      next_non_whitespace();
      auto cond_r = parse_expression();
      if (cond_r.is_err()) {
        return err<R>(std::move(cond_r));
      }
      cond_id = std::move(cond_r).unwrap();
    } else {
      is_else_if = false;
      cond_id = ast::kInvalidNodeId;
    }

    auto block_r = parse_block_expr();
    if (block_r.is_err()) {
      return err<R>(std::move(block_r));
    }

    context_->alloc_payload(ast::IfBranchPayload{
        .condition = cond_id,
        .block = std::move(block_r).unwrap(),
    });

    ++branches_count;

    if (!is_else_if) {
      // last else
      break;
    }
  }

  return ok(context_->alloc_payload(ast::IfExpressionPayload{
      .branches_range = {.begin = first_id, .size = branches_count},
  }));
}

}  // namespace parser
