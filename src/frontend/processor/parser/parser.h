// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_PARSER_PARSER_H_
#define FRONTEND_PROCESSOR_PARSER_PARSER_H_

#include <memory>
#include <string>
#include <vector>

#include "core/check.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/data/ast/base/base_node.h"
#include "frontend/data/ast/nodes/nodes.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/processor/parser/base/parser_export.h"
#include "frontend/processor/parser/parse_error.h"

namespace parser {

class PARSER_EXPORT Parser {
 public:
  using AstNode = ast::AstNode;
  template <typename T>
  using Result = diagnostic::Result<T, ParseError>;
  template <typename T>
  using Results = diagnostic::Result<T, std::vector<ParseError>>;

  explicit Parser(base::TokenStream&& stream, bool strict = false);

  ~Parser() = default;

  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;

  Parser(Parser&&) = default;
  Parser& operator=(Parser&&) = default;

  Results<AstNode> parse();

 private:
  // grammar productions
  Results<AstNode> parse_function();

  Results<AstNode> parse_param_list();
  Results<AstNode> parse_param();

  Result<AstNode> parse_type();

  Results<AstNode> parse_statement();
  Results<AstNode> parse_variable_declaration_statement();
  Results<AstNode> parse_assignment_statement();
  Results<AstNode> parse_let_statement();
  Results<AstNode> parse_return_statement();
  Results<AstNode> parse_if_statement();
  Results<AstNode> parse_while_statement();
  Results<AstNode> parse_for_statement();
  Results<AstNode> parse_block_statement();
  Results<AstNode> parse_expression_statement();

  // expression parsing (precedence climbing)
  Results<AstNode> parse_expression();
  Results<AstNode> parse_assignment();
  Results<AstNode> parse_binary_expression(int min_precedence);
  Results<AstNode> parse_unary_expression();
  Results<AstNode> parse_primary_expression();
  Results<AstNode> parse_function_call(AstNode callee_node);

  // helper methods
  Result<const base::Token*> consume(base::TokenKind expected,
                                     std::string&& error_message);
  inline bool match(base::TokenKind kind) { return stream_.match(kind); }
  inline bool check(base::TokenKind kind) const { return stream_.check(kind); }
  inline bool check(base::TokenKind kind, std::size_t offset) const {
    return stream_.check(kind, offset);
  }
  inline const base::Token& peek(std::size_t offset = 0) const {
    return stream_.peek(offset);
  }
  // TODO: use return value of `stream_.advance()`
  inline void advance() { stream_.advance(); }
  inline const base::Token& previous() const { return stream_.previous(); }

  inline bool eof() const { return stream_.eof(); }

  // error handling
  void synchronize();

  static bool is_sync_point(base::TokenKind kind);

  static Result<AstNode> single_ok(AstNode&& node);
  static Result<AstNode> single_err(ParseError&& error);

  static Results<AstNode> ok(AstNode&& node);
  static Results<AstNode> err(std::vector<ParseError>&& errors);

  static void append_errs(std::vector<ParseError>* target,
                          std::vector<ParseError>&& source);

  base::TokenStream stream_;
  bool strict_ : 1 = false;
};

}  // namespace parser

#endif  // FRONTEND_PROCESSOR_PARSER_PARSER_H_
