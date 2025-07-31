// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// function_call ::= expression "(" [ expression { "," expression } ] ")" ;
// now takes an AstNode for the callee, allowing calls like `obj.method()`
Parser::Results<Parser::AstNode> Parser::parse_function_call(
    AstNode callee_node) {
  std::vector<ParseError> errors;

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' for function call");
  if (lparen_result.is_err()) {
    errors.push_back(std::move(lparen_result).unwrap_err());
    return err(std::move(errors));
  }

  // use the '(' token for the call node's base
  const auto* call_token = std::move(lparen_result).unwrap();

  std::vector<AstNode> arguments;
  if (!check(lexer::TokenKind::kRParen)) {
    auto first_arg_result = parse_expression();
    if (first_arg_result.is_err()) {
      append_errs(&errors, std::move(first_arg_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      }
    } else {
      arguments.push_back(std::move(first_arg_result).unwrap());
    }

    while (match(lexer::TokenKind::kComma)) {
      auto arg_result = parse_expression();
      if (arg_result.is_err()) {
        append_errs(&errors, std::move(arg_result).unwrap_err());
        if (strict_) {
          return err(std::move(errors));
        }
      } else {
        arguments.push_back(std::move(arg_result).unwrap());
      }
    }
  }

  auto rparen_result = consume(lexer::TokenKind::kRParen,
                               "expected ')' after function arguments");
  if (rparen_result.is_err()) {
    errors.push_back(std::move(rparen_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(ast::make_node<ast::CallNode>(*call_token, std::move(callee_node),
                                            std::move(arguments)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
