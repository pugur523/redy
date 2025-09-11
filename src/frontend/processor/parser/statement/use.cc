// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::UseStatementPayload>;

Parser::Result<R> Parser::parse_use_stmt() {
  DCHECK(check(base::TokenKind::kUse));
  next_non_whitespace();

  // TODO: support more path specification pattern like rust
  PayloadId<ast::PathExpressionPayload> first_id;
  uint32_t paths_count;
  if (check(base::TokenKind::kLeftBrace)) {
    // use { some_path::some_func, some_path2::some_func2 }
    paths_count = 0;
    while (!eof() && !check(base::TokenKind::kRightBrace)) {
      auto path_r = parse_path_expr();
      if (path_r.is_err()) {
        return err<R>(std::move(path_r));
      } else if (paths_count == 0) {
        first_id = std::move(path_r).unwrap();
      }
      ++paths_count;

      if (!check(base::TokenKind::kComma)) {
        break;
      }
      // consume comma
      next_non_whitespace();
    }
  } else {
    // use std::some_func
    paths_count = 1;
    auto path_r = parse_path_expr();
    if (path_r.is_err()) {
      return err<R>(std::move(path_r));
    }
    first_id = std::move(path_r).unwrap();
  }

  return ok(context_->alloc_payload(ast::UseStatementPayload{
      .use_paths_range = {.begin = first_id, .size = paths_count}}));
}

}  // namespace parser
