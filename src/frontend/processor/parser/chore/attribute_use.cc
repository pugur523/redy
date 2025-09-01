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

Parser::Result<ast::PayloadId> Parser::parse_attribute_use_one() {
  auto attr_name_r = consume(base::TokenKind::kIdentifier, true);
  if (attr_name_r.is_err()) {
    return err<PayloadId>(std::move(attr_name_r).unwrap_err());
  }
  const PayloadId attribute = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(attr_name_r).unwrap()->lexeme(stream_->file())});

  if (!check(base::TokenKind::kLeftParen)) {
    return ok(context_->alloc(ast::AttributeUsePayload{
        .callee = attribute,
        .args_range = {},
    }));
  }
  auto left_r = consume(base::TokenKind::kLeftParen, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  auto args_r = parse_expression_sequence();
  if (args_r.is_err()) {
    return err<NodeId>(std::move(args_r).unwrap_err());
  }

  auto right_r = consume(base::TokenKind::kRightParen, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->alloc(ast::AttributeUsePayload{
      .callee = attribute,
      .args_range = std::move(args_r).unwrap(),
  }));
}

Parser::Result<ast::PayloadRange> Parser::parse_attribute_use_list() {
  uint32_t captures_count = 0;
  ast::PayloadId id = ast::kInvalidPayloadId;
  while (!eof() && peek().kind() != base::TokenKind::kRightBracket) {
    auto r = parse_attribute_use_one();
    if (r.is_err()) {
      return err<ast::PayloadRange>(std::move(r).unwrap_err());
    } else if (captures_count == 0) {
      id = std::move(r).unwrap();
    }
    ++captures_count;
    next_non_whitespace();
  }

  // returns ok even if id is invalid and captures count is 0
  return ok(ast::PayloadRange{
      .begin = id,
      .size = captures_count,
  });
}

}  // namespace parser
