// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"

namespace parser {

using R = ast::PayloadId<ast::UnionDeclarationPayload>;

Parser::Result<R> Parser::parse_union_decl_stmt(Sad attribute) {
  auto union_r = consume(base::TokenKind::kUnion, true);
  if (union_r.is_err()) {
    return err<R>(std::move(union_r));
  }

  auto union_name_r = parse_path_expr();
  if (union_name_r.is_err()) {
    return err<R>(std::move(union_name_r));
  }

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  auto fields_r = parse_field_list();
  if (fields_r.is_err()) {
    return err<R>(std::move(fields_r));
  }
  const ast::PayloadRange fields_range = std::move(fields_r).unwrap();

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::UnionDeclarationPayload{
      .name = std::move(union_name_r).unwrap(),
      .fields_range = fields_range,
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
