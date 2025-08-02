// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// block_stmt ::= "{" { statement } "}" ;
Parser::Results<Parser::AstNode> Parser::parse_block_statement() {
  std::vector<ParseError> errors;

  auto lbrace_result =
      consume(base::TokenKind::kLeftBrace, "expected '{' to start a block");
  if (lbrace_result.is_err()) {
    errors.push_back(std::move(lbrace_result).unwrap_err());
    return err(std::move(errors));
  }
  const auto* block_token = lbrace_result.unwrap();

  std::vector<AstNode> statements;

  while (!check(base::TokenKind::kRightBrace) && !eof()) {
    auto stmt_result = parse_statement();
    if (stmt_result.is_err()) {
      append_errs(&errors, std::move(stmt_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      }
      synchronize();
      continue;
    }
    statements.push_back(std::move(stmt_result).unwrap());
  }

  auto rbrace_result =
      consume(base::TokenKind::kRightBrace, "expected '}' to end a block");
  if (rbrace_result.is_err()) {
    errors.push_back(std::move(rbrace_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(
        ast::make_node<ast::BlockNode>(*block_token, std::move(statements)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
