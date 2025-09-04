// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"

namespace parser {

using R = ast::PayloadId<ast::FunctionDeclarationPayload>;

Parser::Result<R> Parser::parse_function_decl_stmt(Sad attribute) {
  auto fn_r = consume(base::TokenKind::kFunction, true);
  if (fn_r.is_err()) {
    return err<R>(std::move(fn_r));
  }

  auto fn_name_r = parse_path_expr();
  if (fn_name_r.is_err()) {
    return err<R>(std::move(fn_name_r));
  }
  const PayloadId<ast::PathExpressionPayload> function_name =
      std::move(fn_name_r).unwrap();

  auto lparen_r = consume(base::TokenKind::kLeftParen, true);
  if (lparen_r.is_err()) {
    return err<R>(std::move(lparen_r));
  }

  auto parameters_r = parse_parameter_list();
  if (parameters_r.is_err()) {
    return err<R>(std::move(parameters_r));
  }
  const ast::PayloadRange parameters_range = std::move(parameters_r).unwrap();

  auto rparen_r = consume(base::TokenKind::kRightParen, true);
  if (rparen_r.is_err()) {
    return err<R>(std::move(rparen_r));
  }

  PayloadId<ast::TypeReferencePayload> return_type_id;
  if (check(base::TokenKind::kArrow)) {
    // consumes ->
    next_non_whitespace();
    auto ret_type_r = parse_type_reference();
    if (ret_type_r.is_err()) {
      return err<R>(std::move(ret_type_r));
    }
    return_type_id = std::move(ret_type_r).unwrap();
  }

  // empty body if it's forward declaration
  PayloadId<ast::BlockExpressionPayload> body_id;
  if (check(base::TokenKind::kLeftBrace)) {
    // with definition
    auto body_r = parse_block_expr();
    if (body_r.is_err()) {
      return err<R>(std::move(body_r));
    }
    body_id = std::move(body_r).unwrap();
  }

  return ok(context_->alloc_payload(ast::FunctionDeclarationPayload{
      .name = function_name,
      .parameters_range = parameters_range,
      .return_type = return_type_id,
      .body = body_id,
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
