// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/data/ast/nodes/node_util.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

// binary_op ::= expression ("+" | "-" | "*" | "/" | "^" | ... ) expression ;
Parser::Results<Parser::AstNode> Parser::parse_binary_expression(
    int min_precedence) {
  std::vector<ParseError> errors;

  AstNode left_node;
  auto left_result = parse_unary_expression();
  if (left_result.is_err()) {
    append_errs(&errors, std::move(left_result).unwrap_err());
    return err(std::move(errors));
  }
  left_node = std::move(left_result).unwrap();

  while (!eof()) {
    auto op_kind = peek().kind();
    if (!is_binary_operator(op_kind) || token_is_assignment_operator(op_kind)) {
      break;
    }

    int precedence = binary_operator_precedence(op_kind);
    if (precedence < min_precedence) {
      break;
    }

    const auto& op_token = peek();
    advance();

    int next_min_precedence = precedence + 1;
    if (op_kind == base::TokenKind::kStarStar) {
      next_min_precedence = precedence;
    }

    AstNode right_node;
    auto right_result = parse_binary_expression(next_min_precedence);
    if (right_result.is_err()) {
      append_errs(&errors, std::move(right_result).unwrap_err());
      return err(std::move(errors));
    }
    right_node = std::move(right_result).unwrap();

    base::BinaryOperator op = token_kind_to_binary_op(op_kind);
    if (op == base::BinaryOperator::kUnknown) {
      errors.push_back(ParseError::make(diagnostic::DiagnosticId::kInvalidToken,
                                        op_token, "invalid binary operator"));
      if (strict_) {
        return err(std::move(errors));
      }
    }

    left_node = ast::make_node<ast::BinaryOpNode>(
        op_token, op, std::move(left_node), std::move(right_node));
  }

  if (errors.empty()) {
    return ok(std::move(left_node));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
