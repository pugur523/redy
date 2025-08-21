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

Parser::Result<ast::NodeId> Parser::parse_match_expression() {
  auto match_r = consume(base::TokenKind::kMatch, true);
  if (match_r.is_err()) {
    return err<NodeId>(std::move(match_r).unwrap_err());
  }

  auto match_expr_r = parse_expression();
  if (match_expr_r.is_err()) {
    return match_expr_r;
  }
  const NodeId match_expr_id = std::move(match_expr_r).unwrap();

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  NodeId first_id = ast::kInvalidNodeId;
  uint32_t arms_count = 0;
  while (!eof()) {
    if (peek().kind() == base::TokenKind::kRightBrace) {
      break;
    }

    auto pattern_r = parse_expression();
    if (pattern_r.is_err()) {
      return pattern_r;
    }

    auto arrow_r = consume(base::TokenKind::kArrow, true);
    if (arrow_r.is_err()) {
      return err<NodeId>(std::move(arrow_r).unwrap_err());
    }

    auto expr_r = parse_expression();
    if (expr_r.is_err()) {
      return expr_r;
    }

    context_->alloc(ast::MatchArmPayload{
        .pattern = std::move(pattern_r).unwrap(),
        .expression = std::move(expr_r).unwrap(),
    });
    ++arms_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->create(
      ast::NodeKind::kMatchExpression,
      ast::MatchExpressionPayload{
          .expression = match_expr_id,
          .arms_range = {.begin = first_id, .size = arms_count},
      }));
}

}  // namespace parser
