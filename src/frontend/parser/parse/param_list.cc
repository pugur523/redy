// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// param_list ::= param { "," param } ;
Parser::Results<Parser::AstNode> Parser::parse_param_list() {
  std::vector<AstNode> parameters;
  std::vector<ParseError> errors;

  auto first_param_result = parse_param();
  if (first_param_result.is_err()) {
    return first_param_result;
  }
  parameters.push_back(std::move(first_param_result).unwrap());

  while (match(base::TokenKind::kComma)) {
    auto param_result = parse_param();
    if (param_result.is_err()) {
      append_errs(&errors, std::move(param_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      } else {
        while (!eof() && !check(base::TokenKind::kComma) &&
               !check(base::TokenKind::kRightParen)) {
          advance();
        }
        continue;
      }
    }

    parameters.push_back(std::move(param_result).unwrap());
  }

  if (errors.empty()) {
    return ok(
        ast::make_node<ast::ParameterListNode>(peek(), std::move(parameters)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
