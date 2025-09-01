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

Parser::Result<ast::NodeId> Parser::parse_module_declaration(
    PayloadId attribute) {
  auto module_r = consume(base::TokenKind::kModule, true);
  if (module_r.is_err()) {
    return err<NodeId>(std::move(module_r).unwrap_err());
  }

  auto module_name_r = consume(base::TokenKind::kIdentifier, true);
  if (module_name_r.is_err()) {
    return err<NodeId>(std::move(module_name_r).unwrap_err());
  }
  const PayloadId module_name = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(module_name_r).unwrap()->lexeme(stream_->file()),
  });

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  NodeId first_node = ast::kInvalidNodeId;
  uint32_t nodes_count = 0;
  while (!eof() && !check(base::TokenKind::kRightBrace)) {
    auto stmt_r = parse_statement();
    if (stmt_r.is_err()) {
      return stmt_r;
    }

    if (nodes_count == 0) {
      first_node = std::move(stmt_r).unwrap();
    }
    ++nodes_count;
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->create(
      ast::NodeKind::kModuleDeclaration,
      ast::ModuleDeclarationPayload{
          .name = module_name,
          .module_nodes_range = {.begin = first_node, .size = nodes_count},
          .storage_attribute = attribute,
      }));
}

}  // namespace parser
