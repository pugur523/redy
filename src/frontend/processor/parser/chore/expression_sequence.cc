// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeRange> Parser::parse_expression_sequence() {
  NodeId first_id = ast::kInvalidNodeId;
  uint32_t arg_count = 0;

  while (!eof()) {
    auto arg_value_r = parse_expression();
    if (arg_value_r.is_err()) {
      return err<NodeRange>(std::move(arg_value_r));
    }

    if (arg_count == 0) {
      first_id = std::move(arg_value_r).unwrap();
    }
    ++arg_count;

    if (!check(base::TokenKind::kComma)) {
      break;
    }
    // consume comma
    next_non_whitespace();
  }

  return ok(NodeRange{
      .begin = first_id,
      .size = arg_count,
  });
}

}  // namespace parser
