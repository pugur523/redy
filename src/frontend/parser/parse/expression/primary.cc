// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

Parser::Results<Parser::AstNode> Parser::parse_primary_expression() {
  std::vector<ParseError> errors;

  // handle literal parsing
  if (match(lexer::TokenKind::kLiteralNumeric)) {
    const auto& token = previous();
    return ok(ast::make_node<ast::LiteralNode>(
        token, ast::LiteralNode::Type::kNumeric,
        token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kLiteralChar)) {
    const auto& token = previous();
    return ok(
        ast::make_node<ast::LiteralNode>(token, ast::LiteralNode::Type::kChar,
                                         token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kLiteralStr)) {
    const auto& token = previous();
    return ok(
        ast::make_node<ast::LiteralNode>(token, ast::LiteralNode::Type::kString,
                                         token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kTrue)) {
    const auto& token = previous();
    return ok(ast::make_node<ast::LiteralNode>(token, true));
  } else if (match(lexer::TokenKind::kFalse)) {
    const auto& token = previous();
    return ok(ast::make_node<ast::LiteralNode>(token, false));
  } else if (match(lexer::TokenKind::kIdentifier)) {
    // handle identifiers (could be variable access or function call)
    const auto& token = previous();
    AstNode identifier_node = ast::make_node<ast::IdentifierNode>(
        token, token.lexeme(stream_.file_manager()));
    // check for function call immediately after identifier
    if (check(lexer::TokenKind::kLParen)) {
      return parse_function_call(std::move(identifier_node));
    }

    // just an identifier
    return ok(std::move(identifier_node));
  } else if (match(lexer::TokenKind::kLParen)) {
    // handle parenthesized expressions
    // recursively parse the expression
    // inside parentheses
    auto expr_result = parse_expression();
    if (expr_result.is_err()) {
      append_errs(&errors, std::move(expr_result).unwrap_err());
      return err(std::move(errors));
    }

    auto rparen_result =
        consume(lexer::TokenKind::kRParen, "expected ')' after expression");
    if (rparen_result.is_err()) {
      errors.push_back(std::move(rparen_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      }
    }

    if (errors.empty()) {
      return expr_result;
    } else {
      return err(std::move(errors));
    }
  }

  const auto& token = peek();
  errors.push_back(ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken,
                                    token,
                                    "unexpected token for primary expression"));
  return err(std::move(errors));
}

}  // namespace parser
