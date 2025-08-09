// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/data/ast/nodes/node_util.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

// param ::= identifier ":" type ;
Parser::Results<Parser::AstNode> Parser::parse_param() {
  std::vector<ParseError> errors;

  auto name_token_result =
      consume(base::TokenKind::kIdentifier, "expected parameter name");
  if (name_token_result.is_err()) {
    errors.push_back(std::move(name_token_result).unwrap_err());
    return err(std::move(errors));
  }
  const auto* name_token = std::move(name_token_result).unwrap();
  std::string_view name = name_token->lexeme(stream_.file_manager());

  auto colon_result =
      consume(base::TokenKind::kColon, "expected ':' after parameter name");
  if (colon_result.is_err()) {
    errors.push_back(std::move(colon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  auto type_result = parse_type();
  if (type_result.is_err()) {
    errors.push_back(std::move(type_result).unwrap_err());
    return err(std::move(errors));
  }

  if (errors.empty()) {
    return ok(ast::make_node<ast::ParameterNode>(
        *name_token, name, std::move(type_result).unwrap()));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
