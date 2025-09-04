// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::ParameterPayload>;
using RR = ast::PayloadRange<ast::ParameterPayload>;

Parser::Result<R> Parser::parse_parameter_one() {
  auto param_name_r = parse_path_expr();
  if (param_name_r.is_err()) {
    return err<R>(std::move(param_name_r));
  }
  const PayloadId<ast::PathExpressionPayload> param_name =
      std::move(param_name_r).unwrap();

  auto colon_r = consume(base::TokenKind::kColon, true);
  if (colon_r.is_err()) {
    return err<R>(std::move(colon_r));
  }

  auto type_r = parse_type_reference();
  if (type_r.is_err()) {
    return err<R>(std::move(type_r));
  }
  const PayloadId<ast::TypeReferencePayload> type = std::move(type_r).unwrap();

  return ok(context_->alloc_payload(ast::ParameterPayload{
      .param_name = param_name,
      .type = type,
  }));
}

Parser::Result<RR> Parser::parse_parameter_list() {
  uint32_t parameters_count = 0;
  R id;
  while (!eof() && peek().kind() != base::TokenKind::kRightParen) {
    auto r = parse_parameter_one();
    if (r.is_err()) {
      return err<RR>(std::move(r));
    } else if (parameters_count == 0) {
      id = std::move(r).unwrap();
    }
    ++parameters_count;
    next_non_whitespace();
  }

  // returns ok even if id is invalid and parameters count is 0
  return ok(RR{
      .begin = id,
      .size = parameters_count,
  });
}

}  // namespace parser
