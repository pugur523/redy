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
#include "frontend/lexer/token/token_kind.h"
#include "frontend/lexer/token/token_stream.h"

namespace ast {

Parser::Parser(lexer::TokenStream&& stream) : stream_(std::move(stream)) {}

MultiResult<ASTNode> Parser::parse() {
  return parse_program();
}

Result<ASTNode> Parser::parse_strict() {
  MultiResult<ASTNode> result = parse_program();
  if (result.has_errors()) {
    return Result<ASTNode>(result.errors().front());
  }
  return ok(result.take_value());
}

// program ::= { function | statement } ;
MultiResult<ASTNode> Parser::parse_program() {
  std::vector<ASTNode> statements;
  ParseErrors local_errors;

  while (!eof()) {
    MultiResult<ASTNode> result =
        check(lexer::TokenKind::kFn) ? parse_function() : parse_statement();

    if (result.has_errors()) {
      auto errors = result.take_errors();
      local_errors.insert(local_errors.end(),
                          std::make_move_iterator(errors.begin()),
                          std::make_move_iterator(errors.end()));
      synchronize();
      continue;
    }

    statements.push_back(std::move(result).take_value());
  }

  if (local_errors.empty()) {
    return multi_ok(make_node<ProgramNode>(peek(), std::move(statements)));
  } else {
    return multi_error<ASTNode>(std::move(local_errors));
  }
}

// function ::= "fn" identifier "(" [ param_list ] ")" "->" type "{" { statement
// } "}" ;
MultiResult<ASTNode> Parser::parse_function() {
  auto fn_result = consume(lexer::TokenKind::kFn, "expected 'fn'");
  if (fn_result.has_errors()) {
    return multi_error<ASTNode>(std::move(fn_result.take_errors()));
  }
  const auto& fn_token = fn_result.value();

  auto name_result =
      consume(lexer::TokenKind::kIdentifier, "expected function name");
  if (name_result.has_errors()) {
    return MultiResult<ASTNode>(name_result.take_errors());
  }
  std::string_view name = name_result.value()->lexeme(stream_.file_manager());

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' after function name");
  if (lparen_result.has_errors()) {
    return MultiResult<ASTNode>(lparen_result.take_errors());
  }

  // Parse parameter list
  ASTNode parameters_node;
  if (!check(lexer::TokenKind::kRParen)) {
    auto param_list_result = parse_param_list();
    if (param_list_result.has_errors()) {
      return param_list_result;
    }
    parameters_node = std::move(param_list_result).take_value();
  }

  auto rparen_result =
      consume(lexer::TokenKind::kRParen, "expected ')' after parameters");
  if (rparen_result.has_errors()) {
    return MultiResult<ASTNode>(rparen_result.take_errors());
  }

  auto arrow_result =
      consume(lexer::TokenKind::kArrow, "expected '->' after parameters");
  if (arrow_result.has_errors()) {
    return MultiResult<ASTNode>(arrow_result.take_errors());
  }

  auto return_type_result = parse_type();
  if (return_type_result.has_errors()) {
    return return_type_result;
  }
  ASTNode return_type_node = std::move(return_type_result).take_value();

  auto lbrace_result =
      consume(lexer::TokenKind::kLBrace, "expected '{' before function body");
  if (lbrace_result.has_errors()) {
    return MultiResult<ASTNode>(lbrace_result.take_errors());
  }

  // Parse function body (a block of statements)
  const auto& body_token = peek();
  std::vector<ASTNode> statements;
  ParseErrors local_errors;

  while (!check(lexer::TokenKind::kRBrace) && !eof()) {
    auto stmt_result = parse_statement();
    if (stmt_result.has_errors()) {
      auto errors = stmt_result.take_errors();
      local_errors.insert(local_errors.end(),
                          std::make_move_iterator(errors.begin()),
                          std::make_move_iterator(errors.end()));
      synchronize();
      continue;
    }
    statements.push_back(std::move(stmt_result).take_value());
  }

  auto rbrace_result =
      consume(lexer::TokenKind::kRBrace, "expected '}' after function body");
  if (rbrace_result.has_errors()) {
    auto errors = rbrace_result.take_errors();
    local_errors.insert(local_errors.end(),
                        std::make_move_iterator(errors.begin()),
                        std::make_move_iterator(errors.end()));
  }

  if (!local_errors.empty()) {
    return multi_error<ASTNode>(std::move(local_errors));
  }

  ASTNode body_node = make_node<BlockNode>(body_token, std::move(statements));

  return multi_ok(make_node<FunctionNode>(
      *fn_token, name, std::move(parameters_node), std::move(return_type_node),
      std::move(body_node)));
}

// param_list ::= param { "," param } ;
MultiResult<ASTNode> Parser::parse_param_list() {
  std::vector<ASTNode> parameters;

  auto first_param_result = parse_param();
  if (first_param_result.has_errors()) {
    return first_param_result;
  }
  parameters.push_back(std::move(first_param_result).take_value());

  while (match(lexer::TokenKind::kComma)) {
    auto param_result = parse_param();
    if (param_result.has_errors()) {
      return param_result;
    }
    parameters.push_back(std::move(param_result).take_value());
  }

  return multi_ok(make_node<ParameterListNode>(peek(), std::move(parameters)));
}

// param ::= identifier ":" type ;
MultiResult<ASTNode> Parser::parse_param() {
  auto name_token_result =
      consume(lexer::TokenKind::kIdentifier, "expected parameter name");
  if (name_token_result.has_errors()) {
    return MultiResult<ASTNode>(name_token_result.take_errors());
  }
  const auto* name_token = name_token_result.value();
  std::string_view name = name_token->lexeme(stream_.file_manager());

  auto colon_result =
      consume(lexer::TokenKind::kColon, "expected ':' after parameter name");
  if (colon_result.has_errors()) {
    return MultiResult<ASTNode>(colon_result.take_errors());
  }

  auto type_result = parse_type();
  if (type_result.has_errors()) {
    return type_result;
  }

  return multi_ok(make_node<ParameterNode>(
      *name_token, name, std::move(type_result).take_value()));
}

// type ::=
// "i8" | "i16" | "i32" | "i64" | "i128" |
// "u8" | "u16" | "u32" | "u64" | "u128" |
// "f32" | "f64" | "void" | "bool" | "char" |
// "str" ;
MultiResult<ASTNode> Parser::parse_type() {
  const auto& token = peek();

  if (!check(lexer::TokenKind::kType)) {
    return multi_error<ASTNode>(ParseError(
        "expected a type", token, stream_.file_manager(), stream_.position()));
  }

  std::string type_name = std::string(token.lexeme(stream_.file_manager()));
  advance();

  return multi_ok(make_node<TypeNode>(token, std::move(type_name)));
}

// statement ::= let_stmt | expr_stmt | return_stmt | if_stmt | block_stmt |
// while_stmt | for_stmt ;
MultiResult<ASTNode> Parser::parse_statement() {
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
MultiResult<ASTNode> Parser::parse_variable_declaration_statement() {
  const lexer::Token* mut_token = nullptr;
  if (check(lexer::TokenKind::kMut)) {
    mut_token = &peek();
    advance();
  }

  auto name_result = consume(lexer::TokenKind::kIdentifier,
                             "expected variable name in declaration");
  if (name_result.has_errors()) {
    return MultiResult<ASTNode>(name_result.take_errors());
  }

  const auto* var_token = name_result.value();
  std::string_view name = var_token->lexeme(stream_.file_manager());

  std::optional<ASTNode> type_node;
  std::optional<ASTNode> initializer_expr;

  if (match(lexer::TokenKind::kColon)) {
    // ":" - type specification
    auto type_result = parse_type();
    if (type_result.has_errors()) {
      return type_result;
    }
    type_node = std::move(type_result).take_value();

    if (match(lexer::TokenKind::kEqual)) {
      auto expr_result = parse_expression();
      if (expr_result.has_errors()) {
        return expr_result;
      }
      initializer_expr = std::move(expr_result).take_value();
    }
  } else if (match(lexer::TokenKind::kAssign)) {
    // ":="
    auto expr_result = parse_expression();
    if (expr_result.has_errors()) {
      return expr_result;
    }
    initializer_expr = std::move(expr_result).take_value();
  } else {
    return multi_error<ASTNode>(
        ParseError("expected ':' or ':=' in variable declaration", peek(),
                   stream_.file_manager(), stream_.position()));
  }

  auto semicolon_result = consume(lexer::TokenKind::kSemicolon,
                                  "expected ';' after variable declaration");
  if (semicolon_result.has_errors()) {
    return MultiResult<ASTNode>(semicolon_result.take_errors());
  }

  return multi_ok(make_node<VariableDeclarationNode>(
      *var_token, name, mut_token != nullptr, std::move(type_node),
      std::move(initializer_expr)));
}

// assignment_stmt ::= (identifier | expr) ("=" | "+=" | "-=" | ... ) expression
// ";" ;
MultiResult<ASTNode> Parser::parse_assignment_statement() {
  auto target_result = parse_primary_expression();
  if (target_result.has_errors()) {
    return target_result;
  }

  lexer::TokenKind op_kind = peek().kind();
  if (!is_assignment_operator(op_kind)) {
    return multi_error<ASTNode>(ParseError("expected assignment operator",
                                           peek(), stream_.file_manager(),
                                           stream_.position()));
  }

  const auto& op_token = peek();
  advance();

  auto value_result = parse_expression();
  if (value_result.has_errors()) {
    return value_result;
  }

  auto semicolon_result = consume(lexer::TokenKind::kSemicolon,
                                  "expected ';' after assignment statement");
  if (semicolon_result.has_errors()) {
    return MultiResult<ASTNode>(semicolon_result.take_errors());
  }

  BinaryOpNode::Operator assign_op = token_to_binary_op(op_kind);
  if (assign_op == BinaryOpNode::Operator::kUnknown) {
    return multi_error<ASTNode>(ParseError("Invalid assignment operator",
                                           op_token, stream_.file_manager(),
                                           stream_.position()));
  }

  return multi_ok(make_node<AssignmentNode>(
      op_token, std::move(target_result).take_value(),
      std::move(value_result).take_value(), assign_op));
}

// return_stmt ::= "return" [expression] ";" ;
MultiResult<ASTNode> Parser::parse_return_statement() {
  const auto& return_token = previous();

  std::optional<ASTNode> return_value;
  if (!check(lexer::TokenKind::kSemicolon)) {
    auto expr_result = parse_expression();
    if (expr_result.has_errors()) {
      return expr_result;
    }
    return_value = std::move(expr_result).take_value();
  }

  auto semicolon_result = consume(lexer::TokenKind::kSemicolon,
                                  "expected ';' after return statement");
  if (semicolon_result.has_errors()) {
    return MultiResult<ASTNode>(semicolon_result.take_errors());
  }

  return multi_ok(make_node<ReturnNode>(return_token, std::move(return_value)));
}

// if_stmt ::= "if" expression "{" { statement } "}" [ "else" ( "{" { statement
// } "}" | if_stmt ) ] ;
MultiResult<ASTNode> Parser::parse_if_statement() {
  const auto& if_token = previous();

  auto condition_result = parse_expression();
  if (condition_result.has_errors()) {
    return condition_result;
  }

  auto lbrace_result =
      consume(lexer::TokenKind::kLBrace, "expected '{' after if condition");
  if (lbrace_result.has_errors()) {
    return MultiResult<ASTNode>(lbrace_result.take_errors());
  }

  std::vector<ASTNode> then_statements;
  ParseErrors local_errors;

  while (!check(lexer::TokenKind::kRBrace) && !eof()) {
    auto stmt_result = parse_statement();
    if (stmt_result.has_errors()) {
      auto errors = stmt_result.take_errors();
      local_errors.insert(local_errors.end(),
                          std::make_move_iterator(errors.begin()),
                          std::make_move_iterator(errors.end()));
      synchronize();
      continue;
    }
    then_statements.push_back(std::move(stmt_result).take_value());
  }

  auto rbrace_result =
      consume(lexer::TokenKind::kRBrace, "expected '}' after if body");
  if (rbrace_result.has_errors()) {
    auto errors = rbrace_result.take_errors();
    local_errors.insert(local_errors.end(),
                        std::make_move_iterator(errors.begin()),
                        std::make_move_iterator(errors.end()));
  }

  if (!local_errors.empty()) {
    return multi_error<ASTNode>(std::move(local_errors));
  }

  ASTNode then_block =
      make_node<BlockNode>(*lbrace_result.value(), std::move(then_statements));
  std::optional<ASTNode> else_branch;

  if (match(lexer::TokenKind::kElse)) {
    if (check(lexer::TokenKind::kLBrace)) {
      auto else_block_result = parse_block_statement();
      if (else_block_result.has_errors()) {
        return else_block_result;
      }
      else_branch = std::move(else_block_result).take_value();
    } else if (check(lexer::TokenKind::kIf)) {
      advance();
      auto nested_if_result = parse_if_statement();
      if (nested_if_result.has_errors()) {
        return nested_if_result;
      }
      else_branch = std::move(nested_if_result).take_value();
    } else {
      return multi_error<ASTNode>(
          ParseError("expected '{' or 'if' after 'else'", peek(),
                     stream_.file_manager(), stream_.position()));
    }
  }

  return multi_ok(
      make_node<IfNode>(if_token, std::move(condition_result).take_value(),
                        std::move(then_block), std::move(else_branch)));
}

// while_stmt ::= "while" expression "{" { statement } "}" ;
MultiResult<ASTNode> Parser::parse_while_statement() {
  const auto& while_token = previous();

  auto condition_result = parse_expression();
  if (condition_result.has_errors()) {
    return condition_result;
  }

  auto block_result = parse_block_statement();
  if (block_result.has_errors()) {
    return block_result;
  }

  return multi_ok(make_node<WhileNode>(while_token,
                                       std::move(condition_result).take_value(),
                                       std::move(block_result).take_value()));
}

// for_stmt ::= "for" "(" [init_stmt] ";" [condition_expr] ";" [increment_expr]
// ")" "{" { statement } "}" ;
MultiResult<ASTNode> Parser::parse_for_statement() {
  const auto& for_token = previous();

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' after 'for'");
  if (lparen_result.has_errors()) {
    return MultiResult<ASTNode>(lparen_result.take_errors());
  }

  std::optional<ASTNode> init_stmt;
  std::optional<ASTNode> condition_expr;
  std::optional<ASTNode> increment_expr;

  // Optional initializer
  if (!check(lexer::TokenKind::kSemicolon)) {
    auto init_res = parse_statement();
    if (init_res.has_errors()) {
      return init_res;
    }
    init_stmt = std::move(init_res).take_value();
  }

  auto first_semicolon_result =
      consume(lexer::TokenKind::kSemicolon,
              "expected ';' after for loop initialization");
  if (first_semicolon_result.has_errors()) {
    return MultiResult<ASTNode>(first_semicolon_result.take_errors());
  }

  // Optional condition
  if (!check(lexer::TokenKind::kSemicolon)) {
    auto cond_res = parse_expression();
    if (cond_res.has_errors()) {
      return cond_res;
    }
    condition_expr = std::move(cond_res).take_value();
  }

  auto second_semicolon_result = consume(
      lexer::TokenKind::kSemicolon, "expected ';' after for loop condition");
  if (second_semicolon_result.has_errors()) {
    return MultiResult<ASTNode>(second_semicolon_result.take_errors());
  }

  // Optional increment
  if (!check(lexer::TokenKind::kRParen)) {
    auto incr_res = parse_expression();
    if (incr_res.has_errors()) {
      return incr_res;
    }
    increment_expr = std::move(incr_res).take_value();
  }

  auto rparen_result = consume(lexer::TokenKind::kRParen,
                               "expected ')' after for loop increment");
  if (rparen_result.has_errors()) {
    return MultiResult<ASTNode>(rparen_result.take_errors());
  }

  auto block_result = parse_block_statement();
  if (block_result.has_errors()) {
    return block_result;
  }

  return multi_ok(make_node<ForNode>(
      for_token, std::move(init_stmt), std::move(condition_expr),
      std::move(increment_expr), std::move(block_result).take_value()));
}

// block_stmt ::= "{" { statement } "}" ;
MultiResult<ASTNode> Parser::parse_block_statement() {
  auto lbrace_result =
      consume(lexer::TokenKind::kLBrace, "expected '{' to start a block");
  if (lbrace_result.has_errors()) {
    return MultiResult<ASTNode>(lbrace_result.take_errors());
  }
  const auto* block_token = lbrace_result.value();

  std::vector<ASTNode> statements;
  ParseErrors local_errors;

  while (!check(lexer::TokenKind::kRBrace) && !eof()) {
    auto stmt_result = parse_statement();
    if (stmt_result.has_errors()) {
      auto errors = stmt_result.take_errors();
      local_errors.insert(local_errors.end(),
                          std::make_move_iterator(errors.begin()),
                          std::make_move_iterator(errors.end()));
      synchronize();
      continue;
    }
    statements.push_back(std::move(stmt_result).take_value());
  }

  auto rbrace_result =
      consume(lexer::TokenKind::kRBrace, "expected '}' to end a block");
  if (rbrace_result.has_errors()) {
    auto errors = rbrace_result.take_errors();
    local_errors.insert(local_errors.end(),
                        std::make_move_iterator(errors.begin()),
                        std::make_move_iterator(errors.end()));
  }

  if (!local_errors.empty()) {
    return multi_error<ASTNode>(std::move(local_errors));
  }

  return multi_ok(make_node<BlockNode>(*block_token, std::move(statements)));
}

// expr_stmt ::= expression ";" ;
MultiResult<ASTNode> Parser::parse_expression_statement() {
  auto expr_result = parse_expression();
  if (expr_result.has_errors()) {
    return expr_result;
  }

  auto semicolon_result =
      consume(lexer::TokenKind::kSemicolon, "expected ';' after expression");
  if (semicolon_result.has_errors()) {
    return MultiResult<ASTNode>(semicolon_result.take_errors());
  }

  const auto& token_for_expr_stmt = std::visit(
      [](const auto& node_ptr) -> const lexer::Token& {
        return node_ptr->token;
      },
      expr_result.value());

  return multi_ok(make_node<ExpressionStatementNode>(
      token_for_expr_stmt, std::move(expr_result).take_value()));
}

// expression ::= assignment | binary_op | unary_op | primary_expression ;
MultiResult<ASTNode> Parser::parse_expression() {
  return parse_assignment();
}

MultiResult<ASTNode> Parser::parse_assignment() {
  auto left_expr_result = parse_binary_expression(0);
  if (left_expr_result.has_errors()) {
    return left_expr_result;
  }

  lexer::TokenKind op_kind = peek().kind();
  if (is_assignment_operator(op_kind)) {
    const auto& op_token = peek();
    advance();

    auto right_expr_result = parse_assignment();
    if (right_expr_result.has_errors()) {
      return right_expr_result;
    }

    BinaryOpNode::Operator assign_op = token_to_binary_op(op_kind);
    if (assign_op == BinaryOpNode::Operator::kUnknown) {
      return multi_error<ASTNode>(ParseError("Invalid assignment operator",
                                             op_token, stream_.file_manager(),
                                             stream_.position()));
    }

    return multi_ok(make_node<AssignmentNode>(
        op_token, std::move(left_expr_result).take_value(),
        std::move(right_expr_result).take_value(), assign_op));
  }
  return left_expr_result;
}

// binary_op ::= expression ("+" | "-" | "*" | "/" | "^" | ... ) expression ;
MultiResult<ASTNode> Parser::parse_binary_expression(int min_precedence) {
  ASTNode left_node;
  auto left_result = parse_unary_expression();
  if (left_result.has_errors()) {
    return left_result;
  }
  left_node = std::move(left_result).take_value();

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

    ASTNode right_node;
    auto right_result = parse_binary_expression(next_min_precedence);
    if (right_result.has_errors()) {
      return right_result;
    }
    right_node = std::move(right_result).take_value();

    BinaryOpNode::Operator op = token_to_binary_op(op_kind);
    if (op == BinaryOpNode::Operator::kUnknown) {
      return multi_error<ASTNode>(ParseError("Invalid binary operator",
                                             op_token, stream_.file_manager(),
                                             stream_.position()));
    }

    left_node = make_node<BinaryOpNode>(op_token, op, std::move(left_node),
                                        std::move(right_node));
  }

  return multi_ok(std::move(left_node));
}

// unary_op ::= ("!" | "-" | "++" | "--" | "~") primary_expression ;
MultiResult<ASTNode> Parser::parse_unary_expression() {
  lexer::TokenKind kind = peek().kind();
  if (is_unary_operator(kind)) {
    const auto& op_token = peek();
    advance();

    auto operand_result = parse_unary_expression();
    if (operand_result.has_errors()) {
      return operand_result;
    }

    UnaryOpNode::Operator op = token_to_unary_op(kind);
    if (op == UnaryOpNode::Operator::kUnknown) {
      return multi_error<ASTNode>(ParseError("Invalid unary operator", op_token,
                                             stream_.file_manager(),
                                             stream_.position()));
    }

    return multi_ok(make_node<UnaryOpNode>(
        op_token, op, std::move(operand_result).take_value()));
  }
  return parse_primary_expression();
}

MultiResult<ASTNode> Parser::parse_primary_expression() {
  // Handle literal parsing
  if (match(lexer::TokenKind::kLiteralNumeric)) {
    const auto& token = previous();
    return multi_ok(
        make_node<LiteralNode>(token, LiteralNode::Type::kNumeric,
                               token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kLiteralChar)) {
    const auto& token = previous();
    return multi_ok(make_node<LiteralNode>(
        token, LiteralNode::Type::kChar, token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kLiteralString)) {
    const auto& token = previous();
    return multi_ok(
        make_node<LiteralNode>(token, LiteralNode::Type::kString,
                               token.lexeme(stream_.file_manager())));
  } else if (match(lexer::TokenKind::kTrue)) {
    const auto& token = previous();
    return multi_ok(make_node<LiteralNode>(token, true));
  } else if (match(lexer::TokenKind::kFalse)) {
    const auto& token = previous();
    return multi_ok(make_node<LiteralNode>(token, false));
  } else if (match(lexer::TokenKind::kIdentifier)) {
    // Handle identifiers (could be variable access or function call)
    const auto& token = previous();
    ASTNode identifier_node =
        make_node<IdentifierNode>(token, token.lexeme(stream_.file_manager()));

    // Check for function call immediately after identifier
    if (check(lexer::TokenKind::kLParen)) {
      return parse_function_call(std::move(identifier_node));
    }

    // Just an identifier
    return multi_ok(std::move(identifier_node));
  } else if (match(lexer::TokenKind::kLParen)) {
    // Handle parenthesized expressions
    // Recursively parse the expression
    // inside parentheses
    auto expr_result = parse_expression();
    if (expr_result.has_errors()) {
      return multi_error<ASTNode>(expr_result.take_errors());
    }

    auto rparen_result =
        consume(lexer::TokenKind::kRParen, "expected ')' after expression");
    if (rparen_result.has_errors()) {
      return multi_error<ASTNode>(rparen_result.take_errors());
    }

    return expr_result;
  }

  const auto& token = peek();
  return multi_error<ASTNode>(
      ParseError("Unexpected token for primary expression", token,
                 stream_.file_manager(), stream_.position()));
}

// function_call ::= expression "(" [ expression { "," expression } ] ")" ;
// Now takes an ASTNode for the callee, allowing calls like `obj.method()`
MultiResult<ASTNode> Parser::parse_function_call(ASTNode callee_node) {
  ParseErrors local_errors;

  auto lparen_result =
      consume(lexer::TokenKind::kLParen, "expected '(' for function call");
  if (lparen_result.has_errors()) {
    auto errors = lparen_result.take_errors();
    local_errors.insert(local_errors.end(),
                        std::make_move_iterator(errors.begin()),
                        std::make_move_iterator(errors.end()));
  }

  // Use the '(' token for the call node's base
  const auto* call_token = lparen_result.value();

  std::vector<ASTNode> arguments;
  if (!check(lexer::TokenKind::kRParen)) {
    auto first_arg_result = parse_expression();
    if (first_arg_result.has_errors()) {
      auto errors = first_arg_result.take_errors();
      local_errors.insert(local_errors.end(),
                          std::make_move_iterator(errors.begin()),
                          std::make_move_iterator(errors.end()));
    }
    arguments.push_back(std::move(first_arg_result.value()));

    while (match(lexer::TokenKind::kComma)) {
      auto arg_result = parse_expression();
      if (arg_result.has_errors()) {
        auto errors = arg_result.take_errors();
        local_errors.insert(local_errors.end(),
                            std::make_move_iterator(errors.begin()),
                            std::make_move_iterator(errors.end()));
      }
      arguments.push_back(std::move(first_arg_result.value()));
    }
  }

  auto rparen_result = consume(lexer::TokenKind::kRParen,
                               "expected ')' after function arguments");
  if (rparen_result.has_errors()) {
    auto errors = rparen_result.take_errors();
    local_errors.insert(local_errors.end(),
                        std::make_move_iterator(errors.begin()),
                        std::make_move_iterator(errors.end()));
  }

  if (local_errors.empty()) {
    return multi_ok(make_node<CallNode>(*call_token, std::move(callee_node),
                                        std::move(arguments)));
  } else {
    return multi_error<ASTNode>(std::move(local_errors));
  }
}

// Helper methods
MultiResult<const lexer::Token*> Parser::consume(
    lexer::TokenKind expected,
    const std::string& error_message) {
  const auto& token = peek();
  if (check(expected)) {
    advance();
    return multi_ok(&token);
  }

  return multi_error<const lexer::Token*>(ParseError(
      error_message, token, stream_.file_manager(), stream_.position()));
}

void Parser::synchronize() {
  using Tk = lexer::TokenKind;

  if (!eof()) {
    advance();
  }

  while (!eof()) {
    if (previous().kind() == Tk::kSemicolon) {
      return;  // Stop after a semicolon, likely end of statement
    }

    switch (peek().kind()) {
      // Keywords that typically start a new statement or declaration
      case Tk::kFn:
      case Tk::kIf:
      case Tk::kWhile:
      case Tk::kFor:
      case Tk::kReturn:
      case Tk::kMut:     // For variable declarations
      case Tk::kStruct:  // If you add struct declarations
      case Tk::kEnum:    // If you add enum declarations
        return;
      default: break;
    }

    advance();
  }
}

ParseError Parser::make_error(const std::string& message) const {
  const auto& token = peek();
  return ParseError(message, token, stream_.file_manager(), stream_.position());
}

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
