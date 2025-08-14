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

Parser::Result<ast::ParameterNode> Parser::parse_parameter_one() {
  auto param_name_r = consume(base::TokenKind::kIdentifier);
  if (param_name_r.is_err()) {
    return err<ast::ParameterNode>(std::move(param_name_r).unwrap_err());
  }
  const std::string_view param_name =
      std::move(param_name_r).unwrap()->lexeme(stream_->file());

  auto colon_r = consume(base::TokenKind::kColon);
  if (colon_r.is_err()) {
    return err<ast::ParameterNode>(std::move(colon_r).unwrap_err());
  }

  auto type_r = parse_type_reference();
  if (type_r.is_err()) {
    return err<ast::ParameterNode>(std::move(type_r).unwrap_err());
  }
  ast::NodeId type = context_->alloc(std::move(type_r).unwrap());

  return ok<ast::ParameterNode>({param_name, type});
}

Parser::Result<ast::NodeRange> Parser::parse_parameter_list() {
  uint32_t parameters_count = 0;
  ast::NodeId id = ast::kInvalidNodeId;
  while (!eof() && peek().kind() != base::TokenKind::kRightParen) {
    auto r = parse_parameter_one();
    if (r.is_err()) {
      return err<ast::NodeRange>(std::move(r).unwrap_err());
    }
    if (parameters_count == 0) {
      id = context_->alloc(std::move(r).unwrap());
    } else {
      context_->alloc(std::move(r).unwrap());
    }
    ++parameters_count;
    next();
  }

  return ok<ast::NodeRange>({id, parameters_count});
}

}  // namespace parser
