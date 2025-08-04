// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// expression ::= assignment | binary_op | unary_op | primary_expression ;
Parser::Results<Parser::AstNode> Parser::parse_expression() {
  // TODO: improve this
  return parse_assignment();
}

}  // namespace parser
