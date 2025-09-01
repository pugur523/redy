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

Parser::Result<ast::NodeId> Parser::parse_attribute_statement() {
  auto hash_r = consume(base::TokenKind::kHash, true);
  if (hash_r.is_err()) {
    return err<NodeId>(std::move(hash_r).unwrap_err());
  }

  auto left_r = consume(base::TokenKind::kLeftBracket, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  auto attributes_r = parse_attribute_use_list();
  if (attributes_r.is_err()) {
    return err<NodeId>(std::move(attributes_r).unwrap_err());
  }

  auto right_r = consume(base::TokenKind::kRightBracket, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(
      context_->create(ast::NodeKind::kAttributeStatement,
                       ast::AttributeStatementPayload{
                           .attributes_range = std::move(attributes_r).unwrap(),
                       }));
}

}  // namespace parser
