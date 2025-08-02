// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// statement ::= let_stmt | expr_stmt | return_stmt | if_stmt | block_stmt |
// while_stmt | for_stmt ;
Parser::Results<Parser::AstNode> Parser::parse_statement() {
  if (check(base::TokenKind::kMutable)) {
    // `mut x := ...` or `mut x: type = ...`
    if (check(base::TokenKind::kIdentifier, 1)) {
      auto kind2 = peek(2).kind();
      if (kind2 == base::TokenKind::kColon ||
          kind2 == base::TokenKind::kAssign) {
        return parse_variable_declaration_statement();
      }
    }
  } else if (check(base::TokenKind::kIdentifier)) {
    auto kind1 = peek(1).kind();

    // check for declaration: `x := ...` or `x: type = ...`
    if (kind1 == base::TokenKind::kAssign || kind1 == base::TokenKind::kColon) {
      return parse_variable_declaration_statement();
    }

    // check for assignment: `x = ...`, `x += ...`, etc.
    if (base::token_is_assignment_operator(kind1)) {
      return parse_assignment_statement();
    }
  }

  if (match(base::TokenKind::kReturn)) {
    return parse_return_statement();
  }

  if (match(base::TokenKind::kIf)) {
    return parse_if_statement();
  }

  if (match(base::TokenKind::kWhile)) {
    return parse_while_statement();
  }

  if (match(base::TokenKind::kFor)) {
    return parse_for_statement();
  }

  // block statement
  if (check(base::TokenKind::kLeftBrace)) {
    return parse_block_statement();
  }

  // default to expression statement if none of the above match
  return parse_expression_statement();
}

}  // namespace parser
