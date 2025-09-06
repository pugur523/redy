// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/literal/literal.h"

#include "frontend/base/keyword/keyword.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::LiteralExpressionPayload>;

Parser::Result<R> Parser::parse_literal_expr() {
  const base::Token& literal_token = peek();
  DCHECK(base::token_kind_is_literal(literal_token.kind()));

  const base::LiteralKind literal_kind =
      token_kind_to_literal(literal_token.kind());
  next_non_whitespace();

  return ok(context_->alloc_payload(ast::LiteralExpressionPayload{
      .kind = literal_kind,
      .lexeme_range = literal_token.range(),
  }));
}

}  // namespace parser
