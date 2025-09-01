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

Parser::Result<ast::NodeId> Parser::parse_expression_statement() {
  auto expression_r = parse_expression();
  if (expression_r.is_err()) {
    return err<NodeId>(std::move(expression_r).unwrap_err());
  }
  return ok(context_->create(ast::NodeKind::kExpressionStatement,
                             ast::ExpressionStatementPayload{
                                 .expression = std::move(expression_r).unwrap(),
                             }));
}

}  // namespace parser
