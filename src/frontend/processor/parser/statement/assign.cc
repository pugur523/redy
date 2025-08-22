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

Parser::Result<ast::NodeId> Parser::parse_assign_statement(
    PayloadId attribute) {
  auto target_r = consume(base::TokenKind::kIdentifier, true);
  if (target_r.is_err()) {
    return err<NodeId>(std::move(target_r).unwrap_err());
  }
  const PayloadId identifier_id = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(target_r).unwrap()->lexeme(stream_->file()),
  });

  PayloadId type_id = ast::kInvalidPayloadId;
  switch (peek().kind()) {
    case base::TokenKind::kColonEqual:
    case base::TokenKind::kEqual: next_non_whitespace(); break;
    case base::TokenKind::kColon: {
      next_non_whitespace();

      auto type_r = parse_type_reference();
      if (type_r.is_err()) {
        return type_r;
      }
      type_id = std::move(type_r).unwrap();

      auto equal_r = consume(base::TokenKind::kEqual, true);
      if (equal_r.is_err()) {
        return err<NodeId>(std::move(equal_r).unwrap_err());
      }

      break;
    }
    default: break;
  }
  const PayloadId variable_id = context_->alloc(ast::VariablePayload{
      .identifier_id = identifier_id,
      .type_id = type_id,
  });

  auto value_r = parse_expression();
  if (value_r.is_err()) {
    return value_r;
  }

  return ok(
      context_->create(ast::NodeKind::kAssignStatement,
                       ast::AssignStatementPayload{
                           .target_variable = variable_id,
                           .value_expression = std::move(value_r).unwrap(),
                           .storage_attribute = attribute,
                       }));
}

}  // namespace parser
