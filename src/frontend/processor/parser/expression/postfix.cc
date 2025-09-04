// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_postfix_expr() {
  auto expr_r = parse_unary_expr();
  if (expr_r.is_err()) {
    return err<NodeId>(std::move(expr_r));
  }
  const NodeId expr_id = std::move(expr_r).unwrap();

  switch (peek().kind()) {
    case base::TokenKind::kLeftBracket:
      return wrap_to_node(ast::NodeKind::kIndexExpression,
                          parse_index_expr(expr_id));
    case base::TokenKind::kLeftParen: return parse_function_call_expr(expr_id);
    case base::TokenKind::kHash:
      return wrap_to_node(ast::NodeKind::kFunctionMacroCallExpression,
                          parse_function_macro_call_expr(expr_id));
    case base::TokenKind::kLeftBrace:
      return wrap_to_node(ast::NodeKind::kConstructExpression,
                          parse_construct_expr(expr_id));
    case base::TokenKind::kDot: {
      next_non_whitespace();
      auto symbol_r = parse_path_expr();
      if (symbol_r.is_err()) {
        return err<NodeId>(std::move(symbol_r));
      }
      const PayloadId<ast::PathExpressionPayload> symbol_id =
          std::move(symbol_r).unwrap();

      // some_obj.some_identifier
      const base::Token& postfix = peek();
      switch (postfix.kind()) {
        case base::TokenKind::kLeftParen:
          return parse_method_call_expr(expr_id, symbol_id);
        case base::TokenKind::kHash:
          return wrap_to_node(ast::NodeKind::kMethodMacroCallExpression,
                              parse_method_macro_call_expr(expr_id, symbol_id));
        default:
          return wrap_to_node(ast::NodeKind::kFieldAccessExpression,
                              parse_field_access_expr(expr_id, symbol_id));
      }
    }
    default:
      // raw unary expression
      return ok(expr_id);
  }
}

}  // namespace parser
