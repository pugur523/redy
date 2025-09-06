// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::ExpressionStatementPayload>;

Parser::Result<R> Parser::parse_expression_stmt() {
  auto expression_r = parse_expression();
  if (expression_r.is_err()) {
    return err<R>(std::move(expression_r));
  }
  return ok(context_->alloc_payload(ast::ExpressionStatementPayload{
      .expression = std::move(expression_r).unwrap(),
  }));
}

}  // namespace parser
