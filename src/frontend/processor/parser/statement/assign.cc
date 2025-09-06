// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::AssignStatementPayload>;

Parser::Result<R> Parser::parse_assign_stmt(Sad attribute) {
  auto target_r = parse_path_expr();
  if (target_r.is_err()) {
    return err<R>(std::move(target_r));
  }

  PayloadId<ast::TypeReferencePayload> type_id;

  switch (peek().kind()) {
    case base::TokenKind::kColonEqual:
    case base::TokenKind::kEqual: next_non_whitespace(); break;
    case base::TokenKind::kColon: {
      next_non_whitespace();

      auto type_r = parse_type_reference();
      if (type_r.is_err()) {
        return err<R>(std::move(type_r));
      }
      type_id = std::move(type_r).unwrap();

      auto equal_r = consume(base::TokenKind::kEqual, true);
      if (equal_r.is_err()) {
        return err<R>(std::move(equal_r));
      }

      break;
    }
    default: break;
  }

  auto value_r = parse_expression();
  if (value_r.is_err()) {
    return err<R>(std::move(value_r));
  }

  return ok(context_->alloc_payload(ast::AssignStatementPayload{
      .target_variable = std::move(target_r).unwrap(),
      .target_type = type_id,
      .value_expression = std::move(value_r).unwrap(),
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
