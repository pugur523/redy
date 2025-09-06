// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/data/ast/base/node.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_expression() {
  auto result = parse_range_expr();
  if (result.is_err()) {
    return err<NodeId>(std::move(result));
  } else {
    return ok(context_->alloc(Node{
        .payload_id = std::move(result).unwrap().id,
        .kind = ast::NodeKind::kRangeExpression,
    }));
  }
}

}  // namespace parser
