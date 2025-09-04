// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::FieldAccessExpressionPayload>;

Parser::Result<R> Parser::parse_field_access_expr(
    NodeId obj,
    PayloadId<ast::PathExpressionPayload> field) {
  return ok(context_->alloc_payload(ast::FieldAccessExpressionPayload{
      .obj = obj,
      .field = field,
  }));
}

}  // namespace parser
