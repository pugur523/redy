// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/keyword/type.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/common.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

using R = ast::PayloadId<ast::TypeReferencePayload>;

Parser::Result<R> Parser::parse_type_reference() {
  const base::Token& current = peek();
  const base::TokenKind current_kind = current.kind();

  if (base::token_kind_is_primitive_type(current_kind)) {
    // primitive types

    // consume type
    next_non_whitespace();
    return ok(context_->alloc_payload(ast::TypeReferencePayload(
        base::token_kind_to_primitive_type(current_kind))));
  } else if (current_kind == base::TokenKind::kIdentifier) {
    // user defined types
    auto path_r = parse_path_expr();
    if (path_r.is_err()) {
      return err<R>(std::move(path_r));
    }
    return ok(context_->alloc_payload(
        ast::TypeReferencePayload(std::move(path_r).unwrap())));
  } else if (current_kind == base::TokenKind::kLeftBracket) {
    // array type [i32], [i32; 5]

    // consume [
    next_non_whitespace();

    // recursive
    auto array_type_r = parse_type_reference();
    if (array_type_r.is_err()) {
      return array_type_r;
    }

    PayloadId<ast::ArrayTypePayload> array_id;

    const base::Token& semicolon_or_right = next_non_whitespace();
    if (semicolon_or_right.kind() == base::TokenKind::kRightBracket) {
      array_id = context_->alloc_payload(ast::ArrayTypePayload{
          .type = std::move(array_type_r).unwrap(),
          .array_size_expr = ast::kInvalidNodeId,
      });
    } else {
      auto semicolon_r = consume(base::TokenKind::kSemicolon, false);
      if (semicolon_r.is_err()) {
        return err<R>(std::move(semicolon_r));
      }

      auto array_size_r = parse_expression();
      if (array_size_r.is_err()) {
        return err<R>(std::move(array_size_r));
      }

      array_id = context_->alloc_payload(ast::ArrayTypePayload{
          .type = std::move(array_type_r).unwrap(),
          .array_size_expr = std::move(array_size_r).unwrap(),
      });
    }
    return ok(context_->alloc_payload(ast::TypeReferencePayload(array_id)));
  } else {
    // not primitive, user-defined, or an array -> return error
    return err<R>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagId::kExpectedButFound)
                .label(stream_->file_id(), current.range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis,
                       {translator_->translate(i18n::TranslationKey::kTermType),
                        translator_->translate(
                            base::token_kind_to_tr_key(current_kind))}))
            .build());
  }
}

}  // namespace parser
