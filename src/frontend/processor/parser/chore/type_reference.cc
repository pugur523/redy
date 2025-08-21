// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/keyword/type.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::PayloadId> Parser::parse_type_reference() {
  const base::TokenKind current_kind = peek().kind();
  std::string_view lexeme = "";

  base::TypeKind kind = base::TypeKind::kUnknown;

  if (base::token_kind_is_primitive_type(current_kind)) {
    // primitive types
    kind = token_kind_to_type_kind(current_kind);
    lexeme = peek().lexeme(stream_->file());
    return ok(context_->alloc(ast::TypeReferencePayload{
        .type_name = lexeme,
        .type_kind = kind,
    }));
  } else if (current_kind == base::TokenKind::kIdentifier) {
    // user defined types
    kind = base::TypeKind::kUserDefined;
    lexeme = peek().lexeme(stream_->file());
    return ok(context_->alloc(ast::TypeReferencePayload{
        .type_name = lexeme,
        .type_kind = kind,
    }));
  } else if (current_kind != base::TokenKind::kLeftBracket) {
    // not even an array -> error
    return err<PayloadId>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagId::kExpectedButFound)
                .label(stream_->file_id(), peek().range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis,
                       {translator_->translate(i18n::TranslationKey::kTermType),
                        translator_->translate(
                            base::token_kind_to_tr_key(peek().kind()))}))
            .build());
  }

  // array type [], [i32], [i32; 5]
  const base::Token& type_or_right = next_non_whitespace();
  if (type_or_right.kind() == base::TokenKind::kRightBracket) {
    return ok(context_->alloc(ast::ArrayTypePayload{
        .type = ast::kInvalidPayloadId,
        .array_size = ast::kInvalidPayloadId,
    }));
  }

  // recursive
  auto array_type_r = parse_type_reference();
  if (array_type_r.is_err()) {
    return array_type_r;
  }
  const base::Token& semicolon_or_right = next_non_whitespace();
  if (semicolon_or_right.kind() == base::TokenKind::kRightBracket) {
    return ok(context_->alloc(ast::ArrayTypePayload{
        .type = std::move(array_type_r).unwrap(),
        .array_size = 0,
    }));
  }
  auto semicolon_r = consume(base::TokenKind::kSemicolon, false);
  if (semicolon_r.is_err()) {
    return err<PayloadId>(std::move(semicolon_r).unwrap_err());
  }

  auto array_size_r = parse_unary_expression();
  if (array_size_r.is_err()) {
    return err<PayloadId>(std::move(array_size_r).unwrap_err());
  }

  return ok(context_->alloc(ast::ArrayTypePayload{
      .type = std::move(array_type_r).unwrap(),
      .array_size = std::move(array_size_r).unwrap(),
  }));
}

}  // namespace parser
