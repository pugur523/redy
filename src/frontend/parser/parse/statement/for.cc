// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// for_stmt ::= "for" "(" [init_stmt] ";" [condition_expr] ";" [increment_expr]
// ")" "{" { statement } "}" ;
Parser::Results<Parser::AstNode> Parser::parse_for_statement() {
  std::vector<ParseError> errors;

  const auto& for_token = previous();

  auto lparen_result =
      consume(base::TokenKind::kLeftParen, "expected '(' after 'for'");
  if (lparen_result.is_err()) {
    errors.push_back(std::move(lparen_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  std::optional<AstNode> init_stmt;
  std::optional<AstNode> condition_expr;
  std::optional<AstNode> increment_expr;

  // optional initializer
  if (!check(base::TokenKind::kSemicolon)) {
    auto init_result = parse_statement();
    if (init_result.is_err()) {
      append_errs(&errors, std::move(init_result).unwrap_err());
      return err(std::move(errors));
    }
    init_stmt = std::move(init_result).unwrap();
  }

  auto first_semicolon_result =
      consume(base::TokenKind::kSemicolon,
              "expected ';' after for loop initialization");
  if (first_semicolon_result.is_err()) {
    errors.push_back(std::move(first_semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  // optional condition
  if (!check(base::TokenKind::kSemicolon)) {
    auto cond_result = parse_expression();
    if (cond_result.is_err()) {
      append_errs(&errors, std::move(cond_result).unwrap_err());
      return err(std::move(errors));
    }
    condition_expr = std::move(cond_result).unwrap();
  }

  auto second_semicolon_result = consume(
      base::TokenKind::kSemicolon, "expected ';' after for loop condition");
  if (second_semicolon_result.is_err()) {
    errors.push_back(std::move(second_semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  // optional increment
  if (!check(base::TokenKind::kRightParen)) {
    auto incr_result = parse_expression();
    if (incr_result.is_err()) {
      append_errs(&errors, std::move(incr_result).unwrap_err());
      return err(std::move(errors));
    }
    increment_expr = std::move(incr_result).unwrap();
  }

  auto rparen_result = consume(base::TokenKind::kRightParen,
                               "expected ')' after for loop increment");
  if (rparen_result.is_err()) {
    errors.push_back(std::move(rparen_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  auto block_result = parse_block_statement();
  if (block_result.is_err()) {
    append_errs(&errors, std::move(block_result).unwrap_err());
    return err(std::move(errors));
  }

  if (errors.empty()) {
    return ok(ast::make_node<ast::ForNode>(
        for_token, std::move(init_stmt), std::move(condition_expr),
        std::move(increment_expr), std::move(block_result).unwrap()));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
