// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::ModuleDeclarationPayload>;

Parser::Result<R> Parser::parse_module_decl_stmt(Sad attribute) {
  auto module_r = consume(base::TokenKind::kModule, true);
  if (module_r.is_err()) {
    return err<R>(std::move(module_r));
  }

  auto module_name_r = parse_path_expr();
  if (module_name_r.is_err()) {
    return err<R>(std::move(module_name_r));
  }

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  NodeId first_node = ast::kInvalidNodeId;
  uint32_t nodes_count = 0;

  while (!eof() && !check(base::TokenKind::kRightBrace)) {
    auto stmt_r = parse_statement();
    if (stmt_r.is_err()) {
      return err<R>(std::move(stmt_r));
    }

    if (nodes_count == 0) {
      first_node = std::move(stmt_r).unwrap();
    }
    ++nodes_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::ModuleDeclarationPayload{
      .name = std::move(module_name_r).unwrap(),
      .module_nodes_range = {.begin = first_node, .size = nodes_count},
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
