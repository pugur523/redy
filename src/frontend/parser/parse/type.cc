// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string_view>
#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/parser/parser.h"

namespace parser {

// type ::=
// "i8" | "i16" | "i32" | "i64" | "i128" |
// "u8" | "u16" | "u32" | "u64" | "u128" |
// "f32" | "f64" | "void" | "bool" | "char" |
// "str" ;
Parser::Result<Parser::AstNode> Parser::parse_type() {
  std::vector<ParseError> errors;

  const auto& token = peek();

  if (!check(base::TokenKind::kType)) {
    return single_err(ParseError::make(
        diagnostic::DiagnosticId::kUnexpectedToken, token, "expected a type"));
  } else {
    std::string_view type_name = token.lexeme(stream_.file_manager());
    advance();
    return single_ok(ast::make_node<ast::TypeNode>(token, type_name));
  }
}

}  // namespace parser

