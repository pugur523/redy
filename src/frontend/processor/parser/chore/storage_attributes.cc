// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::StorageAttributeData> Parser::parse_storage_attributes() {
  using TokenKind = base::TokenKind;
  using Sadd = ast::StorageAttributeData::Data;

  const base::Token& first_token = peek();
  TokenKind kind = first_token.kind();

  // read storage attributes
  Sad attribute{};
  const base::Token* attribute_last_token = nullptr;
  while (!eof() && base::token_kind_is_attribute_keyword(kind)) {
    attribute_last_token = &peek();
    switch (kind) {
      case TokenKind::kMutable: attribute |= Sadd::kMutable; break;
      case TokenKind::kConstant: attribute |= Sadd::kConstant; break;
      case TokenKind::kExtern: attribute |= Sadd::kExtern; break;
      case TokenKind::kStatic: attribute |= Sadd::kStatic; break;
      case TokenKind::kThreadLocal: attribute |= Sadd::kThreadLocal; break;
      case TokenKind::kPublic: attribute |= Sadd::kPublic; break;
      case TokenKind::kAsync: attribute |= Sadd::kAsync; break;
      default: break;
    }

    kind = next().kind();
  }

  if (attribute.has_any()) {
    DCHECK(attribute_last_token);
    const core::SourceRange attribute_range{
        first_token.start(),
        attribute_last_token->end(),
    };

    if ((attribute & Sadd::kMutable) && (attribute & Sadd::kConstant)) {
      // for sync point recovery
      stream_->rewind(stream_->position() - 1);

      return err<Sad>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kConflictingStorageSpecifiers)
                  .label(stream_->file_id(), attribute_range,
                         i18n::TranslationKey::
                             kDiagnosticParserConflictingStorageSpecifiers,
                         diagnostic::LabelMarkerType::kLine,
                         {translator_->translate(
                              i18n::TranslationKey::kTermTokenKindMutable),
                          translator_->translate(
                              i18n::TranslationKey::kTermTokenKindConstant)}))
              .build());
    } else if ((attribute & Sadd::kExtern) && (attribute & Sadd::kStatic)) {
      // for sync point recovery
      stream_->rewind(stream_->position() - 1);

      return err<Sad>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kConflictingStorageSpecifiers)
                  .label(stream_->file_id(), attribute_range,
                         i18n::TranslationKey::
                             kDiagnosticParserConflictingStorageSpecifiers,
                         diagnostic::LabelMarkerType::kLine,
                         {translator_->translate(
                              i18n::TranslationKey::kTermTokenKindExtern),
                          translator_->translate(
                              i18n::TranslationKey::kTermTokenKindStatic)}))
              .build());
    }
  }

  return ok<Sad>(attribute);
}

}  // namespace parser
