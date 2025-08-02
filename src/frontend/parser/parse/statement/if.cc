// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// if_stmt ::= "if" expression "{" { statement } "}" [ "else" ( "{" { statement
// } "}" | if_stmt ) ] ;
Parser::Results<Parser::AstNode> Parser::parse_if_statement() {
  std::vector<ParseError> errors;

  const auto& if_token = previous();

  auto condition_result = parse_expression();
  if (condition_result.is_err()) {
    return err({std::move(condition_result).unwrap_err()});
  }

  auto lbrace_result =
      consume(base::TokenKind::kLeftBrace, "expected '{' after if condition");
  if (lbrace_result.is_err()) {
    return err({std::move(lbrace_result).unwrap_err()});
  }

  std::vector<AstNode> then_statements;

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
    }
    then_statements.push_back(std::move(stmt_result).unwrap());
  }

  auto rbrace_result =
      consume(base::TokenKind::kRightBrace, "expected '}' after if body");
  if (rbrace_result.is_err()) {
    errors.push_back(std::move(rbrace_result).unwrap_err());
    return err(std::move(errors));
  }

  AstNode then_block = ast::make_node<ast::BlockNode>(
      *lbrace_result.unwrap(), std::move(then_statements));
  std::optional<AstNode> else_branch;

  if (match(base::TokenKind::kElse)) {
    if (check(base::TokenKind::kLeftBrace)) {
      auto else_block_result = parse_block_statement();
      if (else_block_result.is_err()) {
        append_errs(&errors, std::move(else_block_result).unwrap_err());
        return err(std::move(errors));
      }
      else_branch = std::move(else_block_result).unwrap();
    } else if (check(base::TokenKind::kIf)) {
      advance();
      auto nested_if_result = parse_if_statement();
      if (nested_if_result.is_err()) {
        append_errs(&errors, std::move(nested_if_result).unwrap_err());
        return err(std::move(errors));
      }
      else_branch = std::move(nested_if_result).unwrap();
    } else {
      errors.push_back(
          ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken, peek(),
                           "expected '{' or 'if' after 'else'"));
      if (strict_) {
        return err(std::move(errors));
      }
    }
  }

  if (errors.empty()) {
    return ok(ast::make_node<ast::IfNode>(
        if_token, std::move(condition_result).unwrap(), std::move(then_block),
        std::move(else_branch)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
