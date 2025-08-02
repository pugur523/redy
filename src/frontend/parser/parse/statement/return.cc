// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// return_stmt ::= "ret" [expression] ";" ;
Parser::Results<Parser::AstNode> Parser::parse_return_statement() {
  std::vector<ParseError> errors;

  const auto& return_token = previous();

  std::optional<AstNode> return_value;
  if (!check(base::TokenKind::kSemicolon)) {
    auto expr_result = parse_expression();
    if (expr_result.is_err()) {
      append_errs(&errors, std::move(expr_result).unwrap_err());
      return err(std::move(errors));
    }

    return_value = std::move(expr_result).unwrap();
  }

  auto semicolon_result = consume(base::TokenKind::kSemicolon,
                                  "expected ';' after return statement");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(
        ast::make_node<ast::ReturnNode>(return_token, std::move(return_value)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser

