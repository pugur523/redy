// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_postfix_expression() {
  auto expr = parse_unary_expression();
  if (expr.is_err()) {
    return expr;
  }
  const NodeId expr_id = std::move(expr).unwrap();

  switch (peek().kind()) {
    case base::TokenKind::kColonColon: return parse_path_expression(expr_id);
    case base::TokenKind::kLeftBracket: return parse_index_expression(expr_id);
    case base::TokenKind::kLeftParen:
      return parse_function_call_expression(expr_id);
    case base::TokenKind::kHash:
      return parse_function_macro_call_expression(expr_id);
    case base::TokenKind::kLeftBrace:
      return parse_construct_expression(expr_id);
    case base::TokenKind::kDot: {
      next_non_whitespace();
      auto symbol_r = consume(base::TokenKind::kIdentifier, true);
      if (symbol_r.is_err()) {
        return err<NodeId>(std::move(symbol_r).unwrap_err());
      }
      const NodeId symbol_id = context_->alloc(ast::IdentifierNode{
          .lexeme = std::move(symbol_r).unwrap()->lexeme(stream_->file()),
      });

      // some_obj.some_identifier
      const base::Token& postfix = peek();
      switch (postfix.kind()) {
        case base::TokenKind::kLeftParen:
          return parse_method_call_expression(expr_id, symbol_id);
        case base::TokenKind::kHash:
          return parse_method_macro_call_expression(expr_id, symbol_id);
        default: return parse_field_access_expression(expr_id, symbol_id);
      }
    }
    default:
      // raw identifier
      return ok(expr_id);
  }
}

}  // namespace parser
