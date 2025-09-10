// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::ImplementationDeclarationPayload>;

Parser::Result<R> Parser::parse_impl_decl_stmt(Sad attribute) {
  DCHECK(check(base::TokenKind::kImplementation));
  next_non_whitespace();

  auto first_name_r = parse_path_expr();
  if (first_name_r.is_err()) {
    return err<R>(std::move(first_name_r));
  }

  PayloadId<ast::PathExpressionPayload> target_name;
  PayloadId<ast::PathExpressionPayload> trait_name;
  if (check(base::TokenKind::kFor)) {
    // consume for
    next_non_whitespace();
    auto second_name_r = parse_path_expr();
    if (second_name_r.is_err()) {
      return err<R>(std::move(second_name_r));
    }
    target_name = std::move(second_name_r).unwrap();
    trait_name = std::move(first_name_r).unwrap();
  } else {
    target_name = std::move(first_name_r).unwrap();
  }

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  NodeId first_fn = ast::kInvalidNodeId;
  uint32_t func_count = 0;

  while (!eof() && !check(base::TokenKind::kRightBrace)) {
    auto fn_decl_r = parse_decl_stmt();
    if (fn_decl_r.is_err()) {
      return err<R>(std::move(fn_decl_r));
    }

    if (func_count == 0) {
      first_fn = std::move(fn_decl_r).unwrap();
    }
    ++func_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::ImplementationDeclarationPayload{
      .target_name = target_name,
      .trait_name = trait_name,
      .function_definition_range = {.begin = first_fn, .size = func_count},
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
