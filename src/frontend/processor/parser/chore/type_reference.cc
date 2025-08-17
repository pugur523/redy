// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/type.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_type_reference() {
  const base::TokenKind current_kind = peek().kind();
  const std::string_view lexeme = peek().lexeme(stream_->file());

  base::TypeKind kind = base::TypeKind::kUnknown;
  if (base::token_kind_is_primitive_type(current_kind)) {
    kind = token_kind_to_type_kind(current_kind);
  } else if (current_kind == base::TokenKind::kIdentifier) {
    kind = base::TypeKind::kUserDefined;
  } else {
    return err<ast::NodeId>(
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

  return ok<ast::NodeId>(context_->alloc(ast::TypeReferenceNode{
      .type_name = lexeme,
      .type_kind = kind,
  }));
}

}  // namespace parser
