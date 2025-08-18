// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/literal/literal.h"

#include "frontend/base/keyword/keyword.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_literal_expression() {
  const base::TokenKind kind = peek().kind();
  DCHECK(base::token_kind_is_literal(kind));

  const base::LiteralKind literal_kind = token_kind_to_literal(kind);

  return ok(context_->alloc(ast::LiteralExpressionNode{
      .kind = literal_kind,
      .lexeme = peek().lexeme(stream_->file()),
  }));
}

}  // namespace parser
