// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_field_access_expression(
    NodeId obj,
    NodeId field) {
  return ok(context_->alloc(ast::FieldAccessExpressionNode{
      .obj = obj,
      .field = field,
  }));
}

}  // namespace parser
