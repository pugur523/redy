// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_closure_expression() {
  auto left_r = consume(base::TokenKind::kLeftBracket, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  auto captures_r = parse_capture_list();
  if (captures_r.is_err()) {
    return err<NodeId>(std::move(captures_r).unwrap_err());
  }

  auto right_r = consume(base::TokenKind::kRightBracket, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  auto params_r = parse_parameter_list();
  if (params_r.is_err()) {
    return err<NodeId>(std::move(params_r).unwrap_err());
  }

  auto body_r = parse_expression();
  if (body_r.is_err()) {
    return body_r;
  }

  return ok(
      context_->create(ast::NodeKind::kClosureExpression,
                       ast::ClosureExpressionPayload{
                           .captures_range = std::move(captures_r).unwrap(),
                           .parameters_range = std::move(params_r).unwrap(),
                           .body = std::move(body_r).unwrap(),
                       }));
}

}  // namespace parser
