// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::MatchExpressionPayload>;

Parser::Result<R> Parser::parse_match_expr() {
  auto match_r = consume(base::TokenKind::kMatch, true);
  if (match_r.is_err()) {
    return err<R>(std::move(match_r));
  }

  auto match_expr_r = parse_expression();
  if (match_expr_r.is_err()) {
    return err<R>(std::move(match_expr_r));
  }
  const NodeId match_expr_id = std::move(match_expr_r).unwrap();

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  PayloadId<ast::MatchArmPayload> first_id;
  uint32_t arms_count = 0;

  while (!eof()) {
    if (check(base::TokenKind::kRightBrace)) {
      break;
    }

    auto pattern_r = parse_expression();
    if (pattern_r.is_err()) {
      return err<R>(std::move(pattern_r));
    }

    auto arrow_r = consume(base::TokenKind::kArrow, true);
    if (arrow_r.is_err()) {
      return err<R>(std::move(arrow_r));
    }

    auto expr_r = parse_expression();
    if (expr_r.is_err()) {
      return err<R>(std::move(expr_r));
    }

    const PayloadId<ast::MatchArmPayload> arm_id =
        context_->alloc_payload(ast::MatchArmPayload{
            .pattern = std::move(pattern_r).unwrap(),
            .expression = std::move(expr_r).unwrap(),
        });

    if (arms_count == 0) {
      first_id = arm_id;
    }

    ++arms_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::MatchExpressionPayload{
      .expression = match_expr_id,
      .arms_range = {.begin = first_id, .size = arms_count},
  }));
}

}  // namespace parser
