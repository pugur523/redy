// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::AttributeUsePayload>;
using RR = ast::PayloadRange<ast::AttributeUsePayload>;

Parser::Result<R> Parser::parse_attribute_use_one() {
  auto attr_name_r = parse_path_expr();
  if (attr_name_r.is_err()) {
    return err<R>(std::move(attr_name_r));
  }

  NodeRange args_range{};

  if (check(base::TokenKind::kLeftParen)) {
    // consumes (
    next_non_whitespace();

    auto args_r = parse_expression_sequence();
    if (args_r.is_err()) {
      return err<R>(std::move(args_r));
    }
    args_range = std::move(args_r).unwrap();

    auto right_r = consume(base::TokenKind::kRightParen, true);
    if (right_r.is_err()) {
      return err<R>(std::move(right_r));
    }
  }

  return ok(context_->alloc_payload(ast::AttributeUsePayload{
      .callee = std::move(attr_name_r).unwrap(),
      .args_range = args_range,
  }));
}

Parser::Result<RR> Parser::parse_attribute_use_list() {
  uint32_t captures_count = 0;
  R id;
  while (!eof() && peek().kind() != base::TokenKind::kRightBracket) {
    auto r = parse_attribute_use_one();
    if (r.is_err()) {
      return err<RR>(std::move(r));
    } else if (captures_count == 0) {
      id = std::move(r).unwrap();
    }
    ++captures_count;
    next_non_whitespace();
  }

  // returns ok even if id is invalid and captures count is 0
  return ok(RR{
      .begin = id,
      .size = captures_count,
  });
}

}  // namespace parser
