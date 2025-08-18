// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_parameter_one() {
  auto param_name_r = consume(base::TokenKind::kIdentifier, true);
  if (param_name_r.is_err()) {
    return err<NodeId>(std::move(param_name_r).unwrap_err());
  }
  const std::string_view param_name =
      std::move(param_name_r).unwrap()->lexeme(stream_->file());

  auto colon_r = consume(base::TokenKind::kColon, true);
  if (colon_r.is_err()) {
    return err<NodeId>(std::move(colon_r).unwrap_err());
  }

  auto type_r = parse_type_reference();
  if (type_r.is_err()) {
    return err<NodeId>(std::move(type_r).unwrap_err());
  }
  const NodeId type = std::move(type_r).unwrap();

  return ok(context_->alloc(ast::ParameterNode{
      .name = param_name,
      .type = type,
  }));
}

Parser::Result<ast::NodeRange> Parser::parse_parameter_list() {
  uint32_t parameters_count = 0;
  NodeId id = ast::kInvalidNodeId;
  while (!eof() && peek().kind() != base::TokenKind::kRightParen) {
    auto r = parse_parameter_one();
    if (r.is_err()) {
      return err<ast::NodeRange>(std::move(r).unwrap_err());
    } else if (parameters_count == 0) {
      id = std::move(r).unwrap();
    }
    ++parameters_count;
    next_non_whitespace();
  }

  // returns ok even if id is invalid and parameters count is 0
  return ok(ast::NodeRange{
      .begin = id,
      .size = parameters_count,
  });
}

}  // namespace parser
