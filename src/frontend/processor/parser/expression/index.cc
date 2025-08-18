// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_index_expression() {
  auto operand_r = parse_primary_expression();
  if (operand_r.is_err()) {
    return operand_r;
  }

  auto left_r = consume(base::TokenKind::kLeftBracket, true);
  if (left_r.is_err()) {
    return err<ast::NodeId>(std::move(left_r).unwrap_err());
  }
  auto index_r = parse_expression();
  if (index_r.is_err()) {
    return err<ast::NodeId>(std::move(index_r).unwrap_err());
  }

  auto right_r = consume(base::TokenKind::kRightBracket, true);
  if (right_r.is_err()) {
    return err<ast::NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->alloc(ast::IndexExpressionNode{
      .operand = std::move(operand_r).unwrap(),
      .index = std::move(index_r).unwrap(),
  }));
}

}  // namespace parser
