// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_redirect_declaration(
    PayloadId attribute) {
  auto redirect_r = consume(base::TokenKind::kRedirect, true);
  if (redirect_r.is_err()) {
    return err<NodeId>(std::move(redirect_r).unwrap_err());
  }

  auto fn_name_r = consume(base::TokenKind::kIdentifier, true);
  if (fn_name_r.is_err()) {
    return err<NodeId>(std::move(fn_name_r).unwrap_err());
  }
  const PayloadId fn_name = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(fn_name_r).unwrap()->lexeme(stream_->file()),
  });

  auto arrow_r = consume(base::TokenKind::kArrow, true);
  if (arrow_r.is_err()) {
    return err<NodeId>(std::move(arrow_r).unwrap_err());
  }

  auto target_name_r = consume(base::TokenKind::kIdentifier, true);
  if (target_name_r.is_err()) {
    return err<NodeId>(std::move(target_name_r).unwrap_err());
  }
  const PayloadId target_name = context_->alloc(ast::IdentifierPayload{
      .lexeme = std::move(target_name_r).unwrap()->lexeme(stream_->file()),
  });

  return ok(context_->create(ast::NodeKind::kRedirectDeclaration,
                             ast::RedirectDeclarationPayload{
                                 .name = fn_name,
                                 .target = target_name,
                                 .storage_attribute = attribute,
                             }));
}

}  // namespace parser
