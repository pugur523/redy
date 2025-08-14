// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"

namespace parser {

Parser::Result<ast::FunctionDeclarationNode> Parser::parse_function_declaration(
    ast::BuiltinAttribute attribute) {
  consume(base::TokenKind::kFunction);

  auto fn = consume(base::TokenKind::kIdentifier);
  if (fn.is_err()) {
    return err<ast::FunctionDeclarationNode>(std::move(fn).unwrap_err());
  }
  const std::string_view function_name = peek().lexeme(stream_->file());

  auto lparen_r = consume(base::TokenKind::kLeftParen);
  if (lparen_r.is_err()) {
    return err<ast::FunctionDeclarationNode>(std::move(lparen_r).unwrap_err());
  }

  auto parameters_r = parse_parameter_list();
  if (parameters_r.is_err()) {
    return err<ast::FunctionDeclarationNode>(
        std::move(parameters_r).unwrap_err());
  }
  const ast::NodeRange parameters_range = std::move(parameters_r).unwrap();

  auto rparen_r = consume(base::TokenKind::kRightParen);
  if (rparen_r.is_err()) {
    return err<ast::FunctionDeclarationNode>(std::move(rparen_r).unwrap_err());
  }

  ast::NodeId return_type_id = ast::kInvalidNodeId;
  if (peek().kind() == base::TokenKind::kArrow) {
    consume(base::TokenKind::kArrow);
    auto ret_type_r = parse_type_reference();
    if (ret_type_r.is_err()) {
      return err<ast::FunctionDeclarationNode>(
          std::move(ret_type_r).unwrap_err());
    }
    return_type_id = context_->alloc(std::move(ret_type_r).unwrap());
  }

  auto lbrace_r = consume(base::TokenKind::kLeftBrace);
  if (lbrace_r.is_err()) {
    return err<ast::FunctionDeclarationNode>(std::move(lbrace_r).unwrap_err());
  }

  auto body_r = parse_block_expression();
  if (body_r.is_err()) {
    return err<ast::FunctionDeclarationNode>(std::move(body_r).unwrap_err());
  }
  ast::NodeId body_id = context_->alloc(std::move(body_r).unwrap());

  auto rbrace_r = consume(base::TokenKind::kRightBrace);
  if (rbrace_r.is_err()) {
    return err<ast::FunctionDeclarationNode>(std::move(rbrace_r).unwrap_err());
  }

  return ok(ast::FunctionDeclarationNode{
      .name = function_name,
      .parameters_range = parameters_range,
      .return_type = return_type_id,
      .body = body_id,
      .attribute = attribute,
  });
}

}  // namespace parser
