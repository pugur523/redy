// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_field_access_expression(
    NodeId obj,
    PayloadId field) {
  return ok(context_->create(ast::NodeKind::kFieldAccessExpression,
                             ast::FieldAccessExpressionPayload{
                                 .obj = obj,
                                 .field = field,
                             }));
}

}  // namespace parser
