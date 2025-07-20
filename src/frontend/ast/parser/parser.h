// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_PARSER_PARSER_H_
#define FRONTEND_AST_PARSER_PARSER_H_

#include <memory>
#include <string>
#include <vector>

#include "core/check.h"
#include "frontend/ast/base/ast_export.h"
#include "frontend/ast/base/base_node.h"
#include "frontend/ast/nodes/nodes.h"
#include "frontend/ast/parser/result.h"
#include "frontend/lexer/token/token_stream.h"

namespace ast {

class AST_EXPORT Parser {
 public:
  explicit Parser(lexer::TokenStream&& stream);

  ~Parser() = default;

  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;

  Parser(Parser&&) = default;
  Parser& operator=(Parser&&) = default;

  // Main parsing entry point - collects all errors
  MultiResult<ASTNode> parse();

  // Parse with early termination on first error
  Result<ASTNode> parse_strict();

 private:
  // Grammar productions
  MultiResult<ASTNode> parse_program();
  MultiResult<ASTNode> parse_function();

  MultiResult<ASTNode> parse_param_list();
  MultiResult<ASTNode> parse_param();

  MultiResult<ASTNode> parse_type();

  MultiResult<ASTNode> parse_statement();
  MultiResult<ASTNode> parse_variable_declaration_statement();
  MultiResult<ASTNode> parse_assignment_statement();
  MultiResult<ASTNode> parse_let_statement();
  MultiResult<ASTNode> parse_return_statement();
  MultiResult<ASTNode> parse_if_statement();
  MultiResult<ASTNode> parse_while_statement();
  MultiResult<ASTNode> parse_for_statement();
  MultiResult<ASTNode> parse_block_statement();
  MultiResult<ASTNode> parse_expression_statement();

  // Expression parsing (precedence climbing)
  MultiResult<ASTNode> parse_expression();
  MultiResult<ASTNode> parse_assignment();
  MultiResult<ASTNode> parse_binary_expression(int min_precedence);
  MultiResult<ASTNode> parse_unary_expression();
  MultiResult<ASTNode> parse_primary_expression();
  MultiResult<ASTNode> parse_function_call(ASTNode callee_node);

  // Helper methods
  MultiResult<const lexer::Token*> consume(lexer::TokenKind expected,
                                           const std::string& error_message);
  inline bool match(lexer::TokenKind kind) { return stream_.match(kind); }
  inline bool check(lexer::TokenKind kind) const { return stream_.check(kind); }
  inline bool check(lexer::TokenKind kind, std::size_t offset) const {
    return stream_.check(kind, offset);
  }
  inline const lexer::Token& peek(std::size_t offset = 0) const {
    return stream_.peek(offset);
  }
  // TODO: return token
  inline void advance() { stream_.advance(); }
  inline const lexer::Token& previous() const { return stream_.previous(); }

  inline bool eof() const { return stream_.eof(); }

  // Error handling
  void synchronize();
  ParseError make_error(const std::string& message) const;

  // Token kind to operator conversion
  static BinaryOpNode::Operator token_to_binary_op(lexer::TokenKind kind);
  static UnaryOpNode::Operator token_to_unary_op(lexer::TokenKind kind);

  // Precedence helpers
  static int get_precedence(lexer::TokenKind kind);
  static bool is_binary_operator(lexer::TokenKind kind);
  static bool is_unary_operator(lexer::TokenKind kind);
  static bool is_assignment_operator(lexer::TokenKind kind);

  lexer::TokenStream stream_;
};

}  // namespace ast

#endif  // FRONTEND_AST_PARSER_PARSER_H_
