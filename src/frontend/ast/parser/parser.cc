// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/parser/parser.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/base/logger.h"
#include "core/location.h"
#include "frontend/ast/base/base_node.h"
#include "frontend/ast/nodes/node_util.h"
#include "frontend/ast/parser/parse_error.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/lexer/token/token_kind.h"
#include "frontend/lexer/token/token_stream.h"

namespace ast {

Parser::Parser(lexer::TokenStream&& stream, bool strict)
    : stream_(std::move(stream)), strict_(strict) {}

// program ::= { function | statement } ;
Parser::Results<AstNode> Parser::parse() {
  std::vector<AstNode> statements;
  std::vector<ParseError> errors;

  const auto& start_token = peek();
  while (!eof()) {
    Results<AstNode> result =
        check(lexer::TokenKind::kFn) ? parse_function() : parse_statement();

    if (result.is_err()) {
      append_errs(&errors, std::move(result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      }
      synchronize();
    } else {
      statements.push_back(std::move(result).unwrap());
    }
  }

  if (errors.empty()) {
    return ok(make_node<ProgramNode>(start_token, std::move(statements)));
  } else {
    return err(std::move(errors));
  }
}

// function ::= "fn" identifier "(" [ param_list ] ")" "->" type "{" { statement
// } "}" ;
Parser::Results<AstNode> Parser::parse_function() {
  std::vector<ParseError> errors;

  auto fn_result = consume(lexer::TokenKind::kFn, "expected 'fn'");
  if (fn_result.is_err()) {
    return err({std::move(fn_result).unwrap_err()});
  }
  const auto& fn_token = fn_result.unwrap();

  auto name_result =
      consume(lexer::TokenKind::kIdentifier, "expected function name");
  if (name_result.is_err()) {
    return err({std::move(name_result).unwrap_err()});
  }
  std::string_view name = name_result.unwrap()->lexeme(stream_.file_manager());

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' after function name");
  if (lparen_result.is_err()) {
    return err({std::move(lparen_result).unwrap_err()});
  }

  // Parse parameter list
  AstNode parameters_node;
  if (!check(lexer::TokenKind::kRParen)) {
    auto param_list_result = parse_param_list();
    if (param_list_result.is_err()) {
      return err(std::move(param_list_result).unwrap_err());
    }
    parameters_node = std::move(param_list_result).unwrap();
  }

  auto rparen_result =
      consume(lexer::TokenKind::kRParen, "expected ')' after parameters");
  if (rparen_result.is_err()) {
    return err({std::move(rparen_result).unwrap_err()});
  }

  auto arrow_result =
      consume(lexer::TokenKind::kArrow, "expected '->' after parameters");
  if (arrow_result.is_err()) {
    return err({std::move(arrow_result).unwrap_err()});
  }

  auto return_type_result = parse_type();
  if (return_type_result.is_err()) {
    return err({std::move(return_type_result).unwrap_err()});
  }
  AstNode return_type_node = std::move(return_type_result).unwrap();

  auto lbrace_result =
      consume(lexer::TokenKind::kLBrace, "expected '{' before function body");
  if (lbrace_result.is_err()) {
    return err({std::move(lbrace_result).unwrap_err()});
  }

  // Parse function body (a block of statements)
  const auto& body_token = peek();
  std::vector<AstNode> statements;

  while (!check(lexer::TokenKind::kRBrace) && !eof()) {
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
      consume(lexer::TokenKind::kRBrace, "expected '}' after function body");
  if (rbrace_result.is_err()) {
    errors.push_back(std::move(rbrace_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    AstNode body_node = make_node<BlockNode>(body_token, std::move(statements));
    return ok(make_node<FunctionNode>(
        *fn_token, name, std::move(parameters_node),
        std::move(return_type_node), std::move(body_node)));
  } else {
    return err(std::move(errors));
  }
}

// param_list ::= param { "," param } ;
Parser::Results<AstNode> Parser::parse_param_list() {
  std::vector<AstNode> parameters;
  std::vector<ParseError> errors;

  auto first_param_result = parse_param();
  if (first_param_result.is_err()) {
    return first_param_result;
  }
  parameters.push_back(std::move(first_param_result).unwrap());

  while (match(lexer::TokenKind::kComma)) {
    auto param_result = parse_param();
    if (param_result.is_err()) {
      append_errs(&errors, std::move(param_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      } else {
        while (!eof() && !check(lexer::TokenKind::kComma) &&
               !check(lexer::TokenKind::kRParen)) {
          advance();
        }
        continue;
      }
    }

    parameters.push_back(std::move(param_result).unwrap());
  }

  if (errors.empty()) {
    return ok(make_node<ParameterListNode>(peek(), std::move(parameters)));
  } else {
    return err(std::move(errors));
  }
}

// param ::= identifier ":" type ;
Parser::Results<AstNode> Parser::parse_param() {
  std::vector<ParseError> errors;

  auto name_token_result =
      consume(lexer::TokenKind::kIdentifier, "expected parameter name");
  if (name_token_result.is_err()) {
    errors.push_back(std::move(name_token_result).unwrap_err());
    return err(std::move(errors));
  }
  const auto* name_token = std::move(name_token_result).unwrap();
  std::string_view name = name_token->lexeme(stream_.file_manager());

  auto colon_result =
      consume(lexer::TokenKind::kColon, "expected ':' after parameter name");
  if (colon_result.is_err()) {
    errors.push_back(std::move(colon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  auto type_result = parse_type();
  if (type_result.is_err()) {
    errors.push_back(std::move(type_result).unwrap_err());
    return err(std::move(errors));
  }

  if (errors.empty()) {
    return ok(make_node<ParameterNode>(*name_token, name,
                                       std::move(type_result).unwrap()));
  } else {
    return err(std::move(errors));
  }
}

// type ::=
// "i8" | "i16" | "i32" | "i64" | "i128" |
// "u8" | "u16" | "u32" | "u64" | "u128" |
// "f32" | "f64" | "void" | "bool" | "char" |
// "str" ;
Parser::Result<AstNode> Parser::parse_type() {
  std::vector<ParseError> errors;

  const auto& token = peek();

  if (!check(lexer::TokenKind::kType)) {
    return single_err(ParseError::make(
        diagnostic::DiagnosticId::kUnexpectedToken, token, "expected a type"));
  } else {
    std::string type_name = std::string(token.lexeme(stream_.file_manager()));
    advance();
    return single_ok(make_node<TypeNode>(token, std::move(type_name)));
  }
}

// statement ::= let_stmt | expr_stmt | return_stmt | if_stmt | block_stmt |
// while_stmt | for_stmt ;
Parser::Results<AstNode> Parser::parse_statement() {
  if (check(lexer::TokenKind::kMut)) {
    // `mut x := ...` or `mut x: type = ...`
    if (check(lexer::TokenKind::kIdentifier, 1)) {
      auto kind2 = peek(2).kind();
      if (kind2 == lexer::TokenKind::kColon ||
          kind2 == lexer::TokenKind::kAssign) {
        return parse_variable_declaration_statement();
      }
    }
  } else if (check(lexer::TokenKind::kIdentifier)) {
    auto kind1 = peek(1).kind();

    // Check for declaration: `x := ...` or `x: type = ...`
    if (kind1 == lexer::TokenKind::kAssign ||
        kind1 == lexer::TokenKind::kColon) {
      return parse_variable_declaration_statement();
    }

    // Check for assignment: `x = ...`, `x += ...`, etc.
    if (is_assignment_operator(kind1)) {
      return parse_assignment_statement();
    }
  }

  if (match(lexer::TokenKind::kReturn)) {
    return parse_return_statement();
  }

  if (match(lexer::TokenKind::kIf)) {
    return parse_if_statement();
  }

  if (match(lexer::TokenKind::kWhile)) {
    return parse_while_statement();
  }

  if (match(lexer::TokenKind::kFor)) {
    return parse_for_statement();
  }

  // Block statement
  if (check(lexer::TokenKind::kLBrace)) {
    return parse_block_statement();
  }

  // Default to expression statement if none of the above match
  return parse_expression_statement();
}

// variable_declaration_stmt ::= ["mut"] identifier (":" type ["=" expression] |
// ":=" expression) ";" ;
Parser::Results<AstNode> Parser::parse_variable_declaration_statement() {
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
    return ok(make_node<VariableDeclarationNode>(
        *var_token, name, mut_token != nullptr, std::move(type_node),
        std::move(initializer_expr)));
  } else {
    return err(std::move(errors));
  }
}

// assignment_stmt ::= (identifier | expr) ("=" | "+=" | "-=" | ... ) expression
// ";" ;
Parser::Results<AstNode> Parser::parse_assignment_statement() {
  std::vector<ParseError> errors;

  auto target_result = parse_primary_expression();
  if (target_result.is_err()) {
    return target_result;
  }

  lexer::TokenKind op_kind = peek().kind();
  if (!is_assignment_operator(op_kind)) {
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

  auto semicolon_result = consume(lexer::TokenKind::kSemicolon,
                                  "expected ';' after assignment statement");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  BinaryOpNode::Operator assign_op = token_to_binary_op(op_kind);
  if (assign_op == BinaryOpNode::Operator::kUnknown) {
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
    return ok(
        make_node<AssignmentNode>(op_token, std::move(target_result).unwrap(),
                                  std::move(value_result).unwrap(), assign_op));
  } else {
    return err(std::move(errors));
  }
}

// return_stmt ::= "return" [expression] ";" ;
Parser::Results<AstNode> Parser::parse_return_statement() {
  std::vector<ParseError> errors;

  const auto& return_token = previous();

  std::optional<AstNode> return_value;
  if (!check(lexer::TokenKind::kSemicolon)) {
    auto expr_result = parse_expression();
    if (expr_result.is_err()) {
      append_errs(&errors, std::move(expr_result).unwrap_err());
      return err(std::move(errors));
    }

    return_value = std::move(expr_result).unwrap();
  }

  auto semicolon_result = consume(lexer::TokenKind::kSemicolon,
                                  "expected ';' after return statement");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(make_node<ReturnNode>(return_token, std::move(return_value)));
  } else {
    return err(std::move(errors));
  }
}

// if_stmt ::= "if" expression "{" { statement } "}" [ "else" ( "{" { statement
// } "}" | if_stmt ) ] ;
Parser::Results<AstNode> Parser::parse_if_statement() {
  std::vector<ParseError> errors;

  const auto& if_token = previous();

  auto condition_result = parse_expression();
  if (condition_result.is_err()) {
    return err({std::move(condition_result).unwrap_err()});
  }

  auto lbrace_result =
      consume(lexer::TokenKind::kLBrace, "expected '{' after if condition");
  if (lbrace_result.is_err()) {
    return err({std::move(lbrace_result).unwrap_err()});
  }

  std::vector<AstNode> then_statements;

  while (!check(lexer::TokenKind::kRBrace) && !eof()) {
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
      consume(lexer::TokenKind::kRBrace, "expected '}' after if body");
  if (rbrace_result.is_err()) {
    errors.push_back(std::move(rbrace_result).unwrap_err());
    return err(std::move(errors));
  }

  AstNode then_block =
      make_node<BlockNode>(*lbrace_result.unwrap(), std::move(then_statements));
  std::optional<AstNode> else_branch;

  if (match(lexer::TokenKind::kElse)) {
    if (check(lexer::TokenKind::kLBrace)) {
      auto else_block_result = parse_block_statement();
      if (else_block_result.is_err()) {
        append_errs(&errors, std::move(else_block_result).unwrap_err());
        return err(std::move(errors));
      }
      else_branch = std::move(else_block_result).unwrap();
    } else if (check(lexer::TokenKind::kIf)) {
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
    return ok(make_node<IfNode>(if_token, std::move(condition_result).unwrap(),
                                std::move(then_block), std::move(else_branch)));
  } else {
    return err(std::move(errors));
  }
}

// while_stmt ::= "while" expression "{" { statement } "}" ;
Parser::Results<AstNode> Parser::parse_while_statement() {
  std::vector<ParseError> errors;

  const auto& while_token = previous();

  auto condition_result = parse_expression();
  if (condition_result.is_err()) {
    append_errs(&errors, std::move(condition_result).unwrap_err());
    return err(std::move(errors));
  }

  auto block_result = parse_block_statement();
  if (block_result.is_err()) {
    append_errs(&errors, std::move(block_result).unwrap_err());
    return err(std::move(errors));
  }

  if (errors.empty()) {
    return ok(make_node<WhileNode>(while_token,
                                   std::move(condition_result).unwrap(),
                                   std::move(block_result).unwrap()));

  } else {
    return err(std::move(errors));
  }
}

// for_stmt ::= "for" "(" [init_stmt] ";" [condition_expr] ";" [increment_expr]
// ")" "{" { statement } "}" ;
Parser::Results<AstNode> Parser::parse_for_statement() {
  std::vector<ParseError> errors;

  const auto& for_token = previous();

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' after 'for'");
  if (lparen_result.is_err()) {
    errors.push_back(std::move(lparen_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  std::optional<AstNode> init_stmt;
  std::optional<AstNode> condition_expr;
  std::optional<AstNode> increment_expr;

  // Optional initializer
  if (!check(lexer::TokenKind::kSemicolon)) {
    auto init_result = parse_statement();
    if (init_result.is_err()) {
      append_errs(&errors, std::move(init_result).unwrap_err());
      return err(std::move(errors));
    }
    init_stmt = std::move(init_result).unwrap();
  }

  auto first_semicolon_result =
      consume(lexer::TokenKind::kSemicolon,
              "expected ';' after for loop initialization");
  if (first_semicolon_result.is_err()) {
    errors.push_back(std::move(first_semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  // Optional condition
  if (!check(lexer::TokenKind::kSemicolon)) {
    auto cond_result = parse_expression();
    if (cond_result.is_err()) {
      append_errs(&errors, std::move(cond_result).unwrap_err());
      return err(std::move(errors));
    }
    condition_expr = std::move(cond_result).unwrap();
  }

  auto second_semicolon_result = consume(
      lexer::TokenKind::kSemicolon, "expected ';' after for loop condition");
  if (second_semicolon_result.is_err()) {
    errors.push_back(std::move(second_semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  // Optional increment
  if (!check(lexer::TokenKind::kRParen)) {
    auto incr_result = parse_expression();
    if (incr_result.is_err()) {
      append_errs(&errors, std::move(incr_result).unwrap_err());
      return err(std::move(errors));
    }
    increment_expr = std::move(incr_result).unwrap();
  }

  auto rparen_result = consume(lexer::TokenKind::kRParen,
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
    return ok(make_node<ForNode>(
        for_token, std::move(init_stmt), std::move(condition_expr),
        std::move(increment_expr), std::move(block_result).unwrap()));
  } else {
    return err(std::move(errors));
  }
}

// block_stmt ::= "{" { statement } "}" ;
Parser::Results<AstNode> Parser::parse_block_statement() {
  std::vector<ParseError> errors;

  auto lbrace_result =
      consume(lexer::TokenKind::kLBrace, "expected '{' to start a block");
  if (lbrace_result.is_err()) {
    errors.push_back(std::move(lbrace_result).unwrap_err());
    return err(std::move(errors));
  }
  const auto* block_token = lbrace_result.unwrap();

  std::vector<AstNode> statements;

  while (!check(lexer::TokenKind::kRBrace) && !eof()) {
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
      consume(lexer::TokenKind::kRBrace, "expected '}' to end a block");
  if (rbrace_result.is_err()) {
    errors.push_back(std::move(rbrace_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(make_node<BlockNode>(*block_token, std::move(statements)));
  } else {
    return err(std::move(errors));
  }
}

// expr_stmt ::= expression ";" ;
Parser::Results<AstNode> Parser::parse_expression_statement() {
  std::vector<ParseError> errors;

  auto expr_result = parse_expression();
  if (expr_result.is_err()) {
    append_errs(&errors, std::move(expr_result).unwrap_err());
    return err(std::move(errors));
  }

  auto semicolon_result =
      consume(lexer::TokenKind::kSemicolon, "expected ';' after expression");
  if (semicolon_result.is_err()) {
    errors.push_back(std::move(semicolon_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  const auto& token_for_expr_stmt = std::visit(
      [](const auto& node_ptr) -> const lexer::Token& {
        return node_ptr->token;
      },
      expr_result.unwrap());

  if (errors.empty()) {
    return ok(make_node<ExpressionStatementNode>(
        token_for_expr_stmt, std::move(expr_result).unwrap()));
  } else {
    return err(std::move(errors));
  }
}

// expression ::= assignment | binary_op | unary_op | primary_expression ;
Parser::Results<AstNode> Parser::parse_expression() {
  // TODO: improve this
  return parse_assignment();
}

Parser::Results<AstNode> Parser::parse_assignment() {
  std::vector<ParseError> errors;

  auto left_expr_result = parse_binary_expression(0);
  if (left_expr_result.is_err()) {
    append_errs(&errors, std::move(left_expr_result).unwrap_err());
    return err(std::move(errors));
  }

  lexer::TokenKind op_kind = peek().kind();
  if (is_assignment_operator(op_kind)) {
    const auto& op_token = peek();
    advance();

    auto right_expr_result = parse_assignment();
    if (right_expr_result.is_err()) {
      append_errs(&errors, std::move(right_expr_result).unwrap_err());
      return err(std::move(errors));
    }

    BinaryOpNode::Operator assign_op = token_to_binary_op(op_kind);
    if (assign_op == BinaryOpNode::Operator::kUnknown) {
      errors.push_back(ParseError::make(diagnostic::DiagnosticId::kInvalidToken,
                                        op_token,
                                        "invalid assignment operator"));
      if (strict_) {
        return err(std::move(errors));
      }
    }

    if (errors.empty()) {
      return ok(make_node<AssignmentNode>(
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

// binary_op ::= expression ("+" | "-" | "*" | "/" | "^" | ... ) expression ;
Parser::Results<AstNode> Parser::parse_binary_expression(int min_precedence) {
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
    if (!is_binary_operator(op_kind) || is_assignment_operator(op_kind)) {
      break;
    }

    int precedence = get_precedence(op_kind);
    if (precedence < min_precedence) {
      break;
    }

    const auto& op_token = peek();
    advance();

    int next_min_precedence = precedence + 1;
    if (op_kind == lexer::TokenKind::kStarStar) {
      next_min_precedence = precedence;
    }

    AstNode right_node;
    auto right_result = parse_binary_expression(next_min_precedence);
    if (right_result.is_err()) {
      append_errs(&errors, std::move(right_result).unwrap_err());
      return err(std::move(errors));
    }
    right_node = std::move(right_result).unwrap();

    BinaryOpNode::Operator op = token_to_binary_op(op_kind);
    if (op == BinaryOpNode::Operator::kUnknown) {
      errors.push_back(ParseError::make(diagnostic::DiagnosticId::kInvalidToken,
                                        op_token, "invalid binary operator"));
      if (strict_) {
        return err(std::move(errors));
      }
    }

    left_node = make_node<BinaryOpNode>(op_token, op, std::move(left_node),
                                        std::move(right_node));
  }

  if (errors.empty()) {
    return ok(std::move(left_node));
  } else {
    return err(std::move(errors));
  }
}

// unary_op ::= ("!" | "-" | "++" | "--" | "~") primary_expression ;
Parser::Results<AstNode> Parser::parse_unary_expression() {
  std::vector<ParseError> errors;

  lexer::TokenKind kind = peek().kind();
  if (is_unary_operator(kind)) {
    const auto& op_token = peek();
    advance();

    auto operand_result = parse_unary_expression();
    if (operand_result.is_err()) {
      append_errs(&errors, std::move(operand_result).unwrap_err());
      return err(std::move(errors));
    }

    UnaryOpNode::Operator op = token_to_unary_op(kind);
    if (op == UnaryOpNode::Operator::kUnknown) {
      errors.push_back(ParseError::make(diagnostic::DiagnosticId::kInvalidToken,
                                        op_token, "invalid unary operator"));
      if (strict_) {
        return err(std::move(errors));
      }
    }

    if (errors.empty()) {
      return ok(make_node<UnaryOpNode>(op_token, op,
                                       std::move(operand_result).unwrap()));
    } else {
      return err(std::move(errors));
    }
  }
  return parse_primary_expression();
}

Parser::Results<AstNode> Parser::parse_primary_expression() {
  std::vector<ParseError> errors;

  // Handle literal parsing
  if (match(lexer::TokenKind::kLiteralNumeric)) {
    const auto& token = previous();
    return ok(make_node<LiteralNode>(token, LiteralNode::Type::kNumeric,
                                     token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kLiteralChar)) {
    const auto& token = previous();
    return ok(make_node<LiteralNode>(token, LiteralNode::Type::kChar,
                                     token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kLiteralStr)) {
    const auto& token = previous();
    return ok(make_node<LiteralNode>(token, LiteralNode::Type::kString,
                                     token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kTrue)) {
    const auto& token = previous();
    return ok(make_node<LiteralNode>(token, true));
  } else if (match(lexer::TokenKind::kFalse)) {
    const auto& token = previous();
    return ok(make_node<LiteralNode>(token, false));
  } else if (match(lexer::TokenKind::kIdentifier)) {
    // Handle identifiers (could be variable access or function call)
    const auto& token = previous();
    AstNode identifier_node =
        make_node<IdentifierNode>(token, token.lexeme(stream_.file_manager()));

    // Check for function call immediately after identifier
    if (check(lexer::TokenKind::kLParen)) {
      return parse_function_call(std::move(identifier_node));
    }

    // Just an identifier
    return ok(std::move(identifier_node));
  } else if (match(lexer::TokenKind::kLParen)) {
    // Handle parenthesized expressions
    // Recursively parse the expression
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
                                    "Unexpected token for primary expression"));
  return err(std::move(errors));
}

// function_call ::= expression "(" [ expression { "," expression } ] ")" ;
// Now takes an AstNode for the callee, allowing calls like `obj.method()`
Parser::Results<AstNode> Parser::parse_function_call(AstNode callee_node) {
  std::vector<ParseError> errors;

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' for function call");
  if (lparen_result.is_err()) {
    errors.push_back(std::move(lparen_result).unwrap_err());
    return err(std::move(errors));
  }

  // Use the '(' token for the call node's base
  const auto* call_token = std::move(lparen_result).unwrap();

  std::vector<AstNode> arguments;
  if (!check(lexer::TokenKind::kRParen)) {
    auto first_arg_result = parse_expression();
    if (first_arg_result.is_err()) {
      append_errs(&errors, std::move(first_arg_result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      }
    } else {
      arguments.push_back(std::move(first_arg_result).unwrap());
    }

    while (match(lexer::TokenKind::kComma)) {
      auto arg_result = parse_expression();
      if (arg_result.is_err()) {
        append_errs(&errors, std::move(arg_result).unwrap_err());
        if (strict_) {
          return err(std::move(errors));
        }
      } else {
        arguments.push_back(std::move(arg_result).unwrap());
      }
    }
  }

  auto rparen_result = consume(lexer::TokenKind::kRParen,
                               "expected ')' after function arguments");
  if (rparen_result.is_err()) {
    errors.push_back(std::move(rparen_result).unwrap_err());
    if (strict_) {
      return err(std::move(errors));
    }
  }

  if (errors.empty()) {
    return ok(make_node<CallNode>(*call_token, std::move(callee_node),
                                  std::move(arguments)));
  } else {
    return err(std::move(errors));
  }
}

Parser::Result<const lexer::Token*> Parser::consume(
    lexer::TokenKind expected,
    std::string&& error_message) {
  const auto& token = peek();
  if (check(expected)) {
    advance();
    return Result<const lexer::Token*>(diagnostic::make_ok(&token));
  } else {
    return Result<const lexer::Token*>(diagnostic::make_err(
        ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken, token,
                         std::move(error_message))));
  }
}

void Parser::synchronize() {
  if (eof()) {
    return;
  }
  advance();
  while (!eof()) {
    if (previous().kind() == lexer::TokenKind::kSemicolon) {
      return;
    }
    if (is_sync_point(peek().kind())) {
      return;
    }
    advance();
  }
}

// static
bool Parser::is_sync_point(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;
  switch (kind) {
    case Tk::kFn:
    case Tk::kIf:
    case Tk::kWhile:
    case Tk::kFor:
    case Tk::kReturn:
    case Tk::kMut:
    case Tk::kStruct:
    case Tk::kEnum: return true;
    default: return false;
  }
}

// static
Parser::Result<AstNode> Parser::single_ok(AstNode&& node) {
  return Result<AstNode>(diagnostic::make_ok(std::move(node)));
}

// static
Parser::Result<AstNode> Parser::single_err(ParseError&& error) {
  return Result<AstNode>(diagnostic::make_err(std::move(error)));
}

// static
Parser::Results<AstNode> Parser::ok(AstNode&& node) {
  return Results<AstNode>(diagnostic::make_ok(std::move(node)));
}

// static
Parser::Results<AstNode> Parser::err(std::vector<ParseError>&& errors) {
  return Results<AstNode>(diagnostic::make_err(std::move(errors)));
}

// static
void Parser::append_errs(std::vector<ParseError>* target,
                         std::vector<ParseError>&& source) {
  DCHECK(target);
  target->insert(target->end(), std::make_move_iterator(source.begin()),
                 std::make_move_iterator(source.end()));
}

// ParseError Parser::make_error(const std::string& message) const {
//   const auto& token = peek();
//   return ParseError(message, token, stream_.file_manager(),
//   stream_.position());
// }

// static
BinaryOpNode::Operator Parser::token_to_binary_op(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;
  using Op = BinaryOpNode::Operator;
  switch (kind) {
    // Arithmetic Operators
    case Tk::kPlus: return Op::kAdd;
    case Tk::kMinus: return Op::kSub;
    case Tk::kStar: return Op::kMul;
    case Tk::kSlash: return Op::kDiv;
    case Tk::kPercent: return Op::kMod;
    case Tk::kStarStar: return Op::kPow;

    // Comparison Operators
    case Tk::kEqEq: return Op::kEqual;
    case Tk::kNeq: return Op::kNotEqual;
    case Tk::kLt: return Op::kLess;
    case Tk::kLe: return Op::kLessEqual;
    case Tk::kGt: return Op::kGreater;
    case Tk::kGe: return Op::kGreaterEqual;

    // Logical Operators
    case Tk::kAndAnd: return Op::kAnd;
    case Tk::kPipePipe: return Op::kOr;

    // Bitwise Operators
    case Tk::kAmp: return Op::kBitwiseAnd;    // &
    case Tk::kPipe: return Op::kBitwiseOr;    // |
    case Tk::kCaret: return Op::kBitwiseXor;  // ^
    case Tk::kLtLt: return Op::kLeftShift;    // <<
    case Tk::kGtGt:
      return Op::kRightShift;  // >>

    // Assignment Operators
    case Tk::kPlusEq: return Op::kAddAssign;          // +=
    case Tk::kMinusEq: return Op::kSubAssign;         // -=
    case Tk::kStarEq: return Op::kMulAssign;          // *=
    case Tk::kSlashEq: return Op::kDivAssign;         // /=
    case Tk::kPercentEq: return Op::kModAssign;       // %=
    case Tk::kAmpEq: return Op::kBitwiseAndAssign;    // &=
    case Tk::kPipeEq: return Op::kBitwiseOrAssign;    // |=
    case Tk::kCaretEq: return Op::kBitwiseXorAssign;  // ^=
    case Tk::kLtLtEq: return Op::kLeftShiftAssign;    // <<=
    case Tk::kGtGtEq: return Op::kRightShiftAssign;   // >>=

    default: return Op::kUnknown;
  }
}

// static
UnaryOpNode::Operator Parser::token_to_unary_op(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;
  using Op = UnaryOpNode::Operator;

  switch (kind) {
    case Tk::kBang: return Op::kNot;          // ! (logical NOT)
    case Tk::kMinus: return Op::kNegate;      // - (unary minus)
    case Tk::kTilde: return Op::kBitwiseNot;  // ~ (bitwise NOT)
    case Tk::kPlusPlus: return Op::kIncrement;
    case Tk::kMinusMinus: return Op::kDecrement;

    default: return Op::kUnknown;
  }
}

// Determines operator precedence for binary operators only.
// Higher return value means higher precedence.
// static
int Parser::get_precedence(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;

  switch (kind) {
    case Tk::kPipePipe: return 1;  // || (lowest)
    case Tk::kAndAnd: return 2;    // &&
    case Tk::kEqEq:
    case Tk::kNeq: return 3;  // ==, !=
    case Tk::kLt:
    case Tk::kLe:
    case Tk::kGt:
    case Tk::kGe: return 4;     // <, <=, >, >=
    case Tk::kAmp: return 5;    // & (bitwise AND)
    case Tk::kCaret: return 6;  // ^ (bitwise XOR)
    case Tk::kPipe: return 7;   // | (bitwise OR)
    case Tk::kLtLt:
    case Tk::kGtGt: return 8;  // <<, >>
    case Tk::kPlus:
    case Tk::kMinus: return 9;  // +, -
    case Tk::kStar:
    case Tk::kSlash:
    case Tk::kPercent: return 10;   // *, /, %
    case Tk::kStarStar: return 11;  // ** (highest for binary ops)
    default: return 0;  // Not a binary operator with a specific precedence
  }
}

// Check if a token kind represents a binary operator (excluding assignment
// operators)
// static
bool Parser::is_binary_operator(lexer::TokenKind kind) {
  return get_precedence(kind) > 0;
}

// Check if a token kind represents a unary operator
// static
bool Parser::is_unary_operator(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;

  switch (kind) {
    case Tk::kBang:        // !
    case Tk::kMinus:       // Unary -
    case Tk::kTilde:       // ~
    case Tk::kPlusPlus:    // ++
    case Tk::kMinusMinus:  // --
      return true;
    default: return false;
  }
}

// Check if a token kind represents an assignment operator
// static
bool Parser::is_assignment_operator(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;

  switch (kind) {
    case Tk::kAssign:
    case Tk::kPlusEq:
    case Tk::kMinusEq:
    case Tk::kStarEq:
    case Tk::kSlashEq:
    case Tk::kPercentEq:
    case Tk::kAmpEq:
    case Tk::kPipeEq:
    case Tk::kCaretEq:
    case Tk::kLtLtEq:
    case Tk::kGtGtEq: return true;
    default: return false;
  }
}

}  // namespace ast
