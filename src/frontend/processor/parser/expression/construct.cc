// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::ConstructExpressionPayload>;

Parser::Result<R> Parser::parse_construct_expr(NodeId type_path) {
  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  NodeId first = ast::kInvalidNodeId;
  uint32_t arg_count = 0;

  while (!eof()) {
    if (peek().kind() != base::TokenKind::kDot) {
      break;
    }
    // consume dot
    next_non_whitespace();

    auto field_name_r = parse_path_expr();
    if (field_name_r.is_err()) {
      return err<R>(std::move(field_name_r));
    }

    auto equal_r = consume(base::TokenKind::kEqual, true);
    if (equal_r.is_err()) {
      return err<R>(std::move(equal_r));
    }

    auto init_value_r = parse_expression();
    if (init_value_r.is_err()) {
      return err<R>(std::move(init_value_r));
    }

    if (arg_count == 0) {
      first = std::move(init_value_r).unwrap();
    }
    ++arg_count;

    if (peek().kind() != base::TokenKind::kComma) {
      break;
    }
    // consume comma
    next_non_whitespace();
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::ConstructExpressionPayload{
      .type_path = type_path,
      .args_range = {.begin = first, .size = arg_count},
  }));
}

}  // namespace parser
