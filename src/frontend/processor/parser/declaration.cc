// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "core/base/source_range.h"
#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_decl_stmt() {
  using Kind = base::TokenKind;
  using Sadd = ast::StorageAttributeData::Data;

  const base::Token& first_token = peek();
  Kind kind = first_token.kind();

  // read storage attribute
  Sad attribute{};
  const base::Token* attribute_last_token = nullptr;
  while (!eof() && base::token_kind_is_attribute_keyword(kind)) {
    attribute_last_token = &peek();
    switch (kind) {
      case Kind::kMutable: attribute |= Sadd::kMutable; break;
      case Kind::kConstant: attribute |= Sadd::kConstant; break;
      case Kind::kExtern: attribute |= Sadd::kExtern; break;
      case Kind::kStatic: attribute |= Sadd::kStatic; break;
      case Kind::kThreadLocal: attribute |= Sadd::kThreadLocal; break;
      case Kind::kPublic: attribute |= Sadd::kPublic; break;
      case Kind::kAsync: attribute |= Sadd::kAsync; break;
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

      return err<NodeId>(
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

      return err<NodeId>(
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

  // local macro, undef at the bottom for safety
#define TRY_RETURN_DECL(fn, attr, node_kind)           \
  do {                                                 \
    auto result = fn(attribute);                       \
    if (result.is_err()) {                             \
      return err<NodeId>(std::move(result));           \
    } else {                                           \
      return ok<NodeId>(context_->alloc(Node{          \
          .payload_id = std::move(result).unwrap().id, \
          .kind = node_kind,                           \
      }));                                             \
    }                                                  \
  } while (0);

  switch (kind) {
    case Kind::kFunction: {
      TRY_RETURN_DECL(parse_function_decl_stmt, attribute,
                      ast::NodeKind::kFunctionDeclaration);
    }
    case Kind::kStruct: {
      TRY_RETURN_DECL(parse_struct_decl_stmt, attribute,
                      ast::NodeKind::kStructDeclaration);
    }
    case Kind::kEnumeration: {
      TRY_RETURN_DECL(parse_enumeration_decl_stmt, attribute,
                      ast::NodeKind::kEnumDeclaration);
    }
    case Kind::kTrait: {
      TRY_RETURN_DECL(parse_trait_decl_stmt, attribute,
                      ast::NodeKind::kTraitDeclaration);
    }
    case Kind::kImplementation: {
      TRY_RETURN_DECL(parse_impl_decl_stmt, attribute,
                      ast::NodeKind::kImplDeclaration);
    }
    case Kind::kUnion: {
      TRY_RETURN_DECL(parse_union_decl_stmt, attribute,
                      ast::NodeKind::kUnionDeclaration);
    }
    case Kind::kModule: {
      TRY_RETURN_DECL(parse_module_decl_stmt, attribute,
                      ast::NodeKind::kModuleDeclaration);
    }
    case Kind::kRedirect: {
      TRY_RETURN_DECL(parse_redirect_decl_stmt, attribute,
                      ast::NodeKind::kRedirectDeclaration);
    }
    case Kind::kIdentifier: {
      TRY_RETURN_DECL(parse_assign_stmt, attribute,
                      ast::NodeKind::kAssignStatement);
    }
#undef TRY_RETURN_DECL
    default: {
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kUnexpectedToken)
                  .label(stream_->file_id(), peek().range(),
                         i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                         diagnostic::LabelMarkerType::kLine,
                         {translator_->translate(
                             base::token_kind_to_tr_key(kind))}))
              .build());
    }
  }
}

}  // namespace parser
