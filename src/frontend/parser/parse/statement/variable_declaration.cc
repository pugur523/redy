// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// variable_declaration_stmt ::= ["mut"] identifier (":" type ["=" expression] |
// ":=" expression) ";" ;
Parser::Results<Parser::AstNode>
Parser::parse_variable_declaration_statement() {
  std::vector<ParseError> errors;

  const lexer::Token* mut_token = nullptr;
  if (check(lexer::TokenKind::kMut)) {
    mut_token = &peek();
    advance();
  }

  auto name_result = consume(lexer::TokenKind::kIdentifier,
                             "expected variable name in declaration");
  if (name_result.is_err()) {
    errors.push_back(std::move(name_result).unwrap_err());
    return err(std::move(errors));
  }

  const auto* var_token = name_result.unwrap();
  std::string_view name = var_token->lexeme(stream_.file_manager());

  std::optional<AstNode> type_node;
  std::optional<AstNode> initializer_expr;

  if (match(lexer::TokenKind::kColon)) {
    // ":" - type specification
    auto type_result = parse_type();
    if (type_result.is_err()) {
      errors.push_back(std::move(type_result).unwrap_err());
      return err(std::move(errors));
    }
    type_node = std::move(type_result).unwrap();

    if (match(lexer::TokenKind::kEqual)) {
      auto expr_result = parse_expression();
      if (expr_result.is_err()) {
        append_errs(&errors, std::move(expr_result).unwrap_err());
        return err(std::move(errors));
      }
      initializer_expr = std::move(expr_result).unwrap();
    }
  } else if (match(lexer::TokenKind::kAssign)) {
    // ":="
    auto expr_result = parse_expression();
    if (expr_result.is_err()) {
      append_errs(&errors, std::move(expr_result).unwrap_err());
      return err(std::move(errors));
    }
    initializer_expr = std::move(expr_result).unwrap();
  } else {
    errors.push_back(
        ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken, peek(),
                         "expected ':' or ':=' in variable declaration"));
    if (strict_) {
      return err(std::move(errors));
    }
  }

  auto semicolon_result = consume(lexer::TokenKind::kSemicolon,
                                  "expected ';' after variable declaration");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(ast::make_node<ast::VariableDeclarationNode>(
        *var_token, name, mut_token != nullptr, std::move(type_node),
        std::move(initializer_expr)));
  } else {
    return err(std::move(errors));
  }
}

}  // namespace parser
