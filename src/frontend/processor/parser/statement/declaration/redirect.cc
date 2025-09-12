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

using R = ast::PayloadId<ast::RedirectDeclarationPayload>;

Parser::Result<R> Parser::parse_redirect_decl_stmt(Sad attribute) {
  DCHECK(check(base::TokenKind::kRedirect));
  next_non_whitespace();

  auto fn_name_r = parse_path_expr();
  if (fn_name_r.is_err()) {
    return err<R>(std::move(fn_name_r));
  }

  auto arrow_r = consume(base::TokenKind::kArrow, true);
  if (arrow_r.is_err()) {
    return err<R>(std::move(arrow_r));
  }

  auto target_name_r = parse_path_expr();
  if (target_name_r.is_err()) {
    return err<R>(std::move(target_name_r));
  }

  return ok(context_->alloc_payload(ast::RedirectDeclarationPayload{
      .name = std::move(fn_name_r).unwrap(),
      .target = std::move(target_name_r).unwrap(),
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
