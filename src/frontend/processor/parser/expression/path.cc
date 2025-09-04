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
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"

namespace parser {

using R = ast::PayloadId<ast::PathExpressionPayload>;

Parser::Result<R> Parser::parse_path_expr() {
  const bool is_absolute = check(base::TokenKind::kColonColon);
  if (is_absolute) {
    next_non_whitespace();  // consume ::
  }

  PayloadId<ast::IdentifierPayload> first_part;
  uint32_t parts_count = 0;

  while (!eof()) {
    auto next_part_r = consume(base::TokenKind::kIdentifier, true);
    if (next_part_r.is_err()) {
      return err<R>(std::move(next_part_r));
    }
    const PayloadId<ast::IdentifierPayload> part_id =
        context_->alloc_payload(ast::IdentifierPayload{
            .lexeme_range = std::move(next_part_r).unwrap()->range(),
        });

    if (parts_count == 0) {
      first_part = part_id;
    }

    ++parts_count;

    if (!check(base::TokenKind::kColonColon)) {
      break;
    } else {
      next_non_whitespace();
    }
  }

  if (parts_count == 0) [[unlikely]] {
    // found eof just after ::
    return err<R>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagId::kExpectedButFound)
                .label(stream_->file_id(), peek().range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis,
                       {translator_->translate(
                            i18n::TranslationKey::kTermTokenKindIdentifier),
                        translator_->translate(
                            base::token_kind_to_tr_key(peek().kind()))}))
            .build());
  } else {
    return ok(context_->alloc_payload(ast::PathExpressionPayload{
        .path_parts_range = {first_part, parts_count},
        .is_absolute = is_absolute,
    }));
  }
}

}  // namespace parser
