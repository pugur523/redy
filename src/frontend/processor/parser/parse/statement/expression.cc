// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/data/ast/nodes/node_util.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

// expr_stmt ::= expression ";" ;
Parser::Results<Parser::AstNode> Parser::parse_expression_statement() {
  std::vector<ParseError> errors;

  auto expr_result = parse_expression();
  if (expr_result.is_err()) {
    append_errs(&errors, std::move(expr_result).unwrap_err());
    return err(std::move(errors));
  }

  auto semicolon_result =
      consume(base::TokenKind::kSemicolon, "expected ';' after expression");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  const auto& token_for_expr_stmt = std::visit(
      [](const auto& node_ptr) -> const base::Token& {
        return node_ptr->token;
      },
      expr_result.unwrap());

  if (errors.empty()) {
    return ok(ast::make_node<ast::ExpressionStatementNode>(
        token_for_expr_stmt, std::move(expr_result).unwrap()));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
