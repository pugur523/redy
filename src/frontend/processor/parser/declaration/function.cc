// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_function_declaration(
    PayloadId attribute) {
  auto fn_r = consume(base::TokenKind::kFunction, true);
  if (fn_r.is_err()) {
    return err<NodeId>(std::move(fn_r).unwrap_err());
  }

  auto fn_name_r = consume(base::TokenKind::kIdentifier, true);
  if (fn_name_r.is_err()) {
    return err<NodeId>(std::move(fn_name_r).unwrap_err());
  }
  const PayloadId function_name = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(fn_name_r).unwrap()->lexeme(stream_->file()),
  });

  auto lparen_r = consume(base::TokenKind::kLeftParen, true);
  if (lparen_r.is_err()) {
    return err<NodeId>(std::move(lparen_r).unwrap_err());
  }

  auto parameters_r = parse_parameter_list();
  if (parameters_r.is_err()) {
    return err<NodeId>(std::move(parameters_r).unwrap_err());
  }
  const ast::PayloadRange parameters_range = std::move(parameters_r).unwrap();

  auto rparen_r = consume(base::TokenKind::kRightParen, true);
  if (rparen_r.is_err()) {
    return err<NodeId>(std::move(rparen_r).unwrap_err());
  }

  NodeId return_type_id = ast::kInvalidNodeId;
  if (peek().kind() == base::TokenKind::kArrow) {
    consume(base::TokenKind::kArrow, true);
    auto ret_type_r = parse_type_reference();
    if (ret_type_r.is_err()) {
      return err<NodeId>(std::move(ret_type_r).unwrap_err());
    }
    return_type_id = std::move(ret_type_r).unwrap();
  }

  // forward declaration -> empty body
  NodeId body_id = ast::kInvalidNodeId;
  if (peek().kind() == base::TokenKind::kLeftBrace) {
    // with definition
    auto lbrace_r = consume(base::TokenKind::kLeftBrace, true);
    if (lbrace_r.is_err()) {
      return err<NodeId>(std::move(lbrace_r).unwrap_err());
    }

    auto body_r = parse_block_expression();
    if (body_r.is_err()) {
      return err<NodeId>(std::move(body_r).unwrap_err());
    }
    body_id = std::move(body_r).unwrap();

    auto rbrace_r = consume(base::TokenKind::kRightBrace, true);
    if (rbrace_r.is_err()) {
      return err<NodeId>(std::move(rbrace_r).unwrap_err());
    }
  }

  return ok(context_->create(ast::NodeKind::kFunctionDeclaration,
                             ast::FunctionDeclarationPayload{
                                 .name = function_name,
                                 .parameters_range = parameters_range,
                                 .return_type = return_type_id,
                                 .body = body_id,
                                 .storage_attribute = attribute,
                             }));
}

}  // namespace parser
