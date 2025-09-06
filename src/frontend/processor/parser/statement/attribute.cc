// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::AttributeStatementPayload>;

Parser::Result<R> Parser::parse_attribute_stmt() {
  auto hash_r = consume(base::TokenKind::kHash, true);
  if (hash_r.is_err()) {
    return err<R>(std::move(hash_r));
  }

  auto left_r = consume(base::TokenKind::kLeftBracket, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  auto attributes_r = parse_attribute_use_list();
  if (attributes_r.is_err()) {
    return err<R>(std::move(attributes_r));
  }

  auto right_r = consume(base::TokenKind::kRightBracket, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::AttributeStatementPayload{
      .attributes_range = std::move(attributes_r).unwrap(),
  }));
}

}  // namespace parser
