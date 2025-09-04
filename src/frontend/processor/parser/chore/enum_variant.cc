// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

using R = ast::PayloadId<ast::EnumVariantPayload>;

Parser::Result<R> Parser::parse_enum_variant() {
  auto variant_name_r = parse_path_expr();
  if (variant_name_r.is_err()) {
    return err<R>(std::move(variant_name_r));
  }

  if (check(base::TokenKind::kComma)) {
    // empty
    return ok(context_->alloc_payload(
        ast::EnumVariantPayload(std::move(variant_name_r).unwrap())));
  } else if (check(base::TokenKind::kEqual)) {
    // integer expression
    auto integer_expr_r = parse_expression();
    if (integer_expr_r.is_err()) {
      return err<R>(std::move(integer_expr_r));
    }

    return ok(context_->alloc_payload(
        ast::EnumVariantPayload(std::move(variant_name_r).unwrap(),
                                std::move(integer_expr_r).unwrap())));
  } else if (check(base::TokenKind::kLeftBrace)) {
    // field nodes
    auto field_list_r = parse_field_list();
    if (field_list_r.is_err()) {
      return err<R>(std::move(field_list_r));
    }

    return ok(context_->alloc_payload(ast::EnumVariantPayload(
        std::move(variant_name_r).unwrap(), std::move(field_list_r).unwrap())));
  } else if (check(base::TokenKind::kLeftParen)) {
    // type nodes
    PayloadId<ast::TypeReferencePayload> first_id;
    uint32_t types_count = 0;

    while (!eof() && !check(base::TokenKind::kRightParen)) {
      auto r = parse_type_reference();
      if (r.is_err()) {
        return err<R>(std::move(r));
      } else if (types_count == 0) {
        first_id = std::move(r).unwrap();
      }
      ++types_count;

      if (!check(base::TokenKind::kComma)) {
        break;
      }
      // consume comma
      next_non_whitespace();
    }

    return ok(context_->alloc_payload(
        ast::EnumVariantPayload(std::move(variant_name_r).unwrap(),
                                PayloadRange<ast::TypeReferencePayload>{
                                    .begin = first_id,
                                    .size = types_count,
                                })));
  } else {
    return err<R>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagnosticId::kUnexpectedToken)
                .label(stream_->file_id(), peek().range(),
                       i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                       diagnostic::LabelMarkerType::kLine,
                       {translator_->translate(
                           base::token_kind_to_tr_key(peek().kind()))}))
            .build());
  }
}

}  // namespace parser
