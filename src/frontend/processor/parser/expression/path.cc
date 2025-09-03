// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_path_expression() {
  bool is_absolute = check(base::TokenKind::kColonColon);
  if (is_absolute) {
    next_non_whitespace();  // consumes ::
  }

  PayloadId first_part = ast::kInvalidPayloadId;
  uint32_t parts_count = 0;

  while (!eof()) {
    auto next_part_r = consume(base::TokenKind::kIdentifier, true);
    if (next_part_r.is_err()) {
      return err<NodeId>(std::move(next_part_r).unwrap_err());
    }
    const PayloadId part_id = context_->alloc(ast::IdentifierPayload{
        .lexeme = std::move(next_part_r).unwrap()->lexeme(stream_->file()),
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
    return err<NodeId>(
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
    return ok(
        context_->create(ast::NodeKind::kPathExpression,
                         ast::PathExpressionPayload{
                             .path_parts_range = {first_part, parts_count},
                         }));
  }
}

}  // namespace parser
