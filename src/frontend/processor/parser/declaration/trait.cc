// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::TraitDeclarationPayload>;

Parser::Result<R> Parser::parse_trait_decl_stmt(Sad attribute) {
  auto trait_r = consume(base::TokenKind::kTrait, true);
  if (trait_r.is_err()) {
    return err<R>(std::move(trait_r));
  }

  auto trait_name_r = parse_path_expr();
  if (trait_name_r.is_err()) {
    return err<R>(std::move(trait_name_r));
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

  return ok(context_->alloc_payload(ast::TraitDeclarationPayload{
      .name = std::move(trait_name_r).unwrap(),
      .function_declare_range = {.begin = first_fn, .size = func_count},
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
