// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// while_stmt ::= "while" expression "{" { statement } "}" ;
Parser::Results<Parser::AstNode> Parser::parse_while_statement() {
  std::vector<ParseError> errors;

  const auto& while_token = previous();

  auto condition_result = parse_expression();
  if (condition_result.is_err()) {
    append_errs(&errors, std::move(condition_result).unwrap_err());
    return err(std::move(errors));
  }

  auto block_result = parse_block_statement();
  if (block_result.is_err()) {
    append_errs(&errors, std::move(block_result).unwrap_err());
    return err(std::move(errors));
  }

  if (errors.empty()) {
    return ok(ast::make_node<ast::WhileNode>(
        while_token, std::move(condition_result).unwrap(),
        std::move(block_result).unwrap()));

  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
