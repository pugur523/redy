// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/parser/parser.h"

namespace parser {

// assignment_stmt ::= (identifier | expr) ("=" | "+=" | "-=" | ... ) expression
// ";" ;
Parser::Results<Parser::AstNode> Parser::parse_assignment_statement() {
  std::vector<ParseError> errors;

  auto target_result = parse_primary_expression();
  if (target_result.is_err()) {
    return target_result;
  }

  base::TokenKind op_kind = peek().kind();
  if (!token_is_assignment_operator(op_kind)) {
    errors.push_back(
        ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken, peek(),
                         "expected assignment operator"));
    if (strict_) {
      return err(std::move(errors));
    }
  }

  const auto& op_token = peek();
  advance();

  auto value_result = parse_expression();
  if (value_result.is_err()) {
    append_errs(&errors, std::move(value_result).unwrap_err());
    return err(std::move(errors));
  }

  auto semicolon_result = consume(base::TokenKind::kSemicolon,
                                  "expected ';' after assignment statement");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  base::BinaryOperator assign_op = token_kind_to_binary_op(op_kind);
  if (assign_op == base::BinaryOperator::kUnknown) {
    errors.push_back(
        ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken, op_token,
                         "invalid assignment operator"));
    if (strict_) {
      return err(std::move(errors));
    }
  }

  // FIXME: `parse_primary_expression()` allows literals like `1 = x`,
  //        but assignment targets should be limited to l-values (e.g.,
  //        identifiers, deref, etc.)
  //        -> we need to write `parse_lvalue_expression` or smth and use it

  if (errors.empty()) {
    return ok(ast::make_node<ast::AssignmentNode>(
        op_token, std::move(target_result).unwrap(),
        std::move(value_result).unwrap(), assign_op));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
