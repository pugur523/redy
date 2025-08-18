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

Parser::Result<ast::NodeId> Parser::parse_path_expression(NodeId first_part) {
  uint32_t parts_count = 1;
  bool seen_identifier = true;
  while (!eof()) {
    if (seen_identifier) {
      if (peek().kind() != base::TokenKind::kColonColon) {
        break;
      }
      seen_identifier = false;
    } else {
      auto next_part_r = consume(base::TokenKind::kIdentifier, true);
      if (next_part_r.is_err()) {
        return err<NodeId>(std::move(next_part_r).unwrap_err());
      }
      context_->alloc(ast::IdentifierNode{
          .lexeme = std::move(next_part_r).unwrap()->lexeme(stream_->file()),
      });
      ++parts_count;
      seen_identifier = true;
    }
    next_non_whitespace();
  }

  return ok(context_->alloc(ast::PathExpressionNode{
      .path_parts_range = {first_part, parts_count},
  }));
}

}  // namespace parser
