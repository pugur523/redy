// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// function ::= "fn" identifier "(" [ param_list ] ")" "->" type "{" { statement
// } "}" ;
Parser::Results<Parser::AstNode> Parser::parse_function() {
  std::vector<ParseError> errors;

  auto fn_result = consume(base::TokenKind::kFunction, "expected 'fn'");
  if (fn_result.is_err()) {
    return err({std::move(fn_result).unwrap_err()});
  }
  const auto& fn_token = fn_result.unwrap();

  auto name_result =
      consume(base::TokenKind::kIdentifier, "expected function name");
  if (name_result.is_err()) {
    return err({std::move(name_result).unwrap_err()});
  }
  std::string_view name = name_result.unwrap()->lexeme(stream_.file_manager());

  auto lparen_result =
      consume(base::TokenKind::kLeftParen, "expected '(' after function name");
  if (lparen_result.is_err()) {
    return err({std::move(lparen_result).unwrap_err()});
  }

  // parse parameter list
  AstNode parameters_node;
  if (!check(base::TokenKind::kRightParen)) {
    auto param_list_result = parse_param_list();
    if (param_list_result.is_err()) {
      return err(std::move(param_list_result).unwrap_err());
    }
    parameters_node = std::move(param_list_result).unwrap();
  }

  auto rparen_result =
      consume(base::TokenKind::kRightParen, "expected ')' after parameters");
  if (rparen_result.is_err()) {
    return err({std::move(rparen_result).unwrap_err()});
  }

  auto arrow_result =
      consume(base::TokenKind::kArrow, "expected '->' after parameters");
  if (arrow_result.is_err()) {
    return err({std::move(arrow_result).unwrap_err()});
  }

  auto return_type_result = parse_type();
  if (return_type_result.is_err()) {
    return err({std::move(return_type_result).unwrap_err()});
  }
  AstNode return_type_node = std::move(return_type_result).unwrap();

  auto lbrace_result =
      consume(base::TokenKind::kLeftBrace, "expected '{' before function body");
  if (lbrace_result.is_err()) {
    return err({std::move(lbrace_result).unwrap_err()});
  }

  // parse function body (a block of statements)
  const auto& body_token = peek();
  std::vector<AstNode> statements;

  while (!check(base::TokenKind::kRightBrace) && !eof()) {
    auto stmt_result = parse_statement();
    if (stmt_result.is_err()) {
      append_errs(&errors, std::move(stmt_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      } else {
        synchronize();
        continue;
      }
    } else {
      statements.push_back(std::move(stmt_result).unwrap());
    }
  }

  auto rbrace_result =
      consume(base::TokenKind::kRightBrace, "expected '}' after function body");
  if (rbrace_result.is_err()) {
    errors.push_back(std::move(rbrace_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    AstNode body_node =
        ast::make_node<ast::BlockNode>(body_token, std::move(statements));
    return ok(ast::make_node<ast::FunctionNode>(
        *fn_token, name, std::move(parameters_node),
        std::move(return_type_node), std::move(body_node)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
