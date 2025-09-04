// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::ForExpressionPayload>;

Parser::Result<R> Parser::parse_for_expr() {
  // 1. for i: 0..<100 { println#("{}", i) }
  // 2. for x: vec { println#("{}", x.value) }

  auto for_r = consume(base::TokenKind::kFor, true);
  if (for_r.is_err()) {
    return err<R>(std::move(for_r));
  }

  auto iterator_r = parse_path_expr();
  if (iterator_r.is_err()) {
    return err<R>(std::move(iterator_r));
  }

  auto colon_r = consume(base::TokenKind::kColon, true);
  if (colon_r.is_err()) {
    return err<R>(std::move(colon_r));
  }

  // TODO: iterator type annotation

  auto range_r = parse_expression();
  if (range_r.is_err()) {
    return err<R>(std::move(range_r));
  }
  const NodeId range_id = std::move(range_r).unwrap();

  auto block_r = parse_block_expr();
  if (block_r.is_err()) {
    return err<R>(std::move(block_r));
  }

  return ok(context_->alloc_payload(ast::ForExpressionPayload{
      .iterator = std::move(iterator_r).unwrap(),
      .range = range_id,
      .body = std::move(block_r).unwrap(),
  }));
}

}  // namespace parser
