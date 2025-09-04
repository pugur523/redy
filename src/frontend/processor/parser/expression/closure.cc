// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

using R = ast::PayloadId<ast::ClosureExpressionPayload>;

Parser::Result<R> Parser::parse_closure_expr() {
  auto left_r = consume(base::TokenKind::kLeftBracket, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  auto captures_r = parse_capture_list();
  if (captures_r.is_err()) {
    return err<R>(std::move(captures_r));
  }

  auto right_r = consume(base::TokenKind::kRightBracket, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  auto params_r = parse_parameter_list();
  if (params_r.is_err()) {
    return err<R>(std::move(params_r));
  }

  auto body_r = parse_expression();
  if (body_r.is_err()) {
    return err<R>(std::move(body_r));
  }

  return ok(context_->alloc_payload(ast::ClosureExpressionPayload{
      .captures_range = std::move(captures_r).unwrap(),
      .parameters_range = std::move(params_r).unwrap(),
      .body = std::move(body_r).unwrap(),
  }));
}

}  // namespace parser
