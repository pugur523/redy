// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::EnumerationDeclarationPayload>;

Parser::Result<R> Parser::parse_enumeration_decl_stmt(Sad attribute) {
  auto enumeration_r = consume(base::TokenKind::kEnumeration, true);
  if (enumeration_r.is_err()) {
    return err<R>(std::move(enumeration_r));
  }

  auto enumeration_name_r = parse_path_expr();
  if (enumeration_name_r.is_err()) {
    return err<R>(std::move(enumeration_name_r));
  }

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<R>(std::move(left_r));
  }

  PayloadId<ast::EnumVariantPayload> first_variant;
  uint32_t variants_count = 0;

  while (!eof() && !check(base::TokenKind::kRightBrace)) {
    auto enum_variant_r = parse_enum_variant();
    if (enum_variant_r.is_err()) {
      return err<R>(std::move(enum_variant_r));
    }

    if (variants_count == 0) {
      first_variant = std::move(enum_variant_r).unwrap();
    }
    ++variants_count;

    if (!check(base::TokenKind::kComma)) {
      break;
    }
    // consume comma
    next_non_whitespace();
  }

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<R>(std::move(right_r));
  }

  return ok(context_->alloc_payload(ast::EnumerationDeclarationPayload{
      .name = std::move(enumeration_name_r).unwrap(),
      .variants_range = {.begin = first_variant, .size = variants_count},
      .storage_attribute = attribute,
  }));
}

}  // namespace parser
