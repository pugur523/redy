// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/data/ast/nodes/node_util.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

// unary_op ::= ("!" | "-" | "++" | "--" | "~") primary_expression ;
Parser::Results<Parser::AstNode> Parser::parse_unary_expression() {
  std::vector<ParseError> errors;

  base::TokenKind kind = peek().kind();
  if (is_unary_operator(kind)) {
    const auto& op_token = peek();
    advance();

    auto operand_result = parse_unary_expression();
    if (operand_result.is_err()) {
      append_errs(&errors, std::move(operand_result).unwrap_err());
      return err(std::move(errors));
    }

    base::UnaryOperator op = token_kind_to_unary_op(kind);
    if (op == base::UnaryOperator::kUnknown) {
      errors.push_back(ParseError::make(diagnostic::DiagnosticId::kInvalidToken,
                                        op_token, "invalid unary operator"));
      if (strict_) {
        return err(std::move(errors));
      }
    }

    if (errors.empty()) {
      return ok(ast::make_node<ast::UnaryOpNode>(
          op_token, op, std::move(operand_result).unwrap()));
    } else {
      return err(std::move(errors));
    }
  }
  return parse_primary_expression();
}

}  // namespace parser
