// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::MethodMacroCallExpressionPayload>;

Parser::Result<R> Parser::parse_method_macro_call_expr(
    NodeId obj,
    PayloadId<ast::PathExpressionPayload> method) {
  auto hash_r = consume(base::TokenKind::kHash, true);
  if (hash_r.is_err()) {
    return err<R>(std::move(hash_r));
  }

  auto left_r = consume(base::TokenKind::kLeftParen, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  auto args_r = parse_expression_sequence();
  if (args_r.is_err()) {
    return err<R>(std::move(args_r));
  }

  auto right_r = consume(base::TokenKind::kRightParen, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::MethodMacroCallExpressionPayload{
      .obj = obj,
      .macro_method = method,
      .args_range = std::move(args_r).unwrap(),
  }));
}

}  // namespace parser
