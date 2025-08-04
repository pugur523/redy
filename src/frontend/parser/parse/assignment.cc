// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/parser/parser.h"

namespace parser {

Parser::Results<Parser::AstNode> Parser::parse_assignment() {
  std::vector<ParseError> errors;

  auto left_expr_result = parse_binary_expression(0);
  if (left_expr_result.is_err()) {
    append_errs(&errors, std::move(left_expr_result).unwrap_err());
    return err(std::move(errors));
  }

  base::TokenKind op_kind = peek().kind();
  if (base::token_is_assignment_operator(op_kind)) {
    const auto& op_token = peek();
    advance();

    auto right_expr_result = parse_assignment();
    if (right_expr_result.is_err()) {
      append_errs(&errors, std::move(right_expr_result).unwrap_err());
      return err(std::move(errors));
    }

    base::BinaryOperator assign_op = base::token_kind_to_binary_op(op_kind);
    if (assign_op == base::BinaryOperator::kUnknown) {
      errors.push_back(ParseError::make(diagnostic::DiagnosticId::kInvalidToken,
                                        op_token,
                                        "invalid assignment operator"));
      if (strict_) {
        return err(std::move(errors));
      }
    }

    if (errors.empty()) {
      return ok(ast::make_node<ast::AssignmentNode>(
          op_token, std::move(left_expr_result).unwrap(),
          std::move(right_expr_result).unwrap(), assign_op));
    } else {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return left_expr_result;
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
