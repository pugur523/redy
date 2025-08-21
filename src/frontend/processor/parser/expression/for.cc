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

Parser::Result<ast::NodeId> Parser::parse_for_expression() {
  // 1. for i: 0..<100 { println#("{}", i) }
  // 2. for x: vec { println#("{}", x.value) }

  auto for_r = consume(base::TokenKind::kFor, true);
  if (for_r.is_err()) {
    return err<NodeId>(std::move(for_r).unwrap_err());
  }

  auto iterator_r = consume(base::TokenKind::kIdentifier, true);
  if (iterator_r.is_err()) {
    return err<NodeId>(std::move(iterator_r).unwrap_err());
  }
  const PayloadId iterator_id = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(iterator_r).unwrap()->lexeme(stream_->file()),
  });

  auto colon_r = consume(base::TokenKind::kColon, true);
  if (colon_r.is_err()) {
    return err<NodeId>(std::move(colon_r).unwrap_err());
  }

  // TODO: iterator type annotation

  auto range_r = parse_expression();
  if (range_r.is_err()) {
    return range_r;
  }
  const NodeId range_id = std::move(range_r).unwrap();

  auto block_r = parse_block_expression();
  if (block_r.is_err()) {
    return block_r;
  }
  const NodeId block_id = std::move(block_r).unwrap();

  return ok(context_->create(ast::NodeKind::kForExpression,
                             ast::ForExpressionPayload{
                                 .iterator = iterator_id,
                                 .range = range_id,
                                 .body = block_id,
                             }));
}

}  // namespace parser
