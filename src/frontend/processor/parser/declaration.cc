// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "core/base/source_range.h"
#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_declaration() {
  using Kind = base::TokenKind;

  const base::Token& first_token = peek();
  Kind kind = first_token.kind();

  // read storage attribute
  StAt attribute{};
  while (!eof() && base::token_kind_is_attribute_keyword(kind)) {
    switch (kind) {
      case Kind::kMutable: attribute.is_mutable = true; break;
      case Kind::kConstant: attribute.is_const = true; break;
      case Kind::kExtern: attribute.is_extern = true; break;
      case Kind::kStatic: attribute.is_static = true; break;
      case Kind::kThreadLocal: attribute.is_thread_local = true; break;
      case Kind::kPublic: attribute.is_public = true; break;
      case Kind::kAsync: attribute.is_async = true; break;
      default: break;
    }

    kind = next().kind();
  }

  const base::Token& current = peek();

  if (has_any_storage_attribute(attribute)) {
    if (attribute.is_mutable && attribute.is_const) [[unlikely]] {
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kUnexpectedToken)
                  .label(stream_->file_id(),
                         core::SourceRange{
                             core::SourceLocation{
                                 first_token.start().line(),
                                 first_token.start().column(),
                             },
                             core::SourceLocation{
                                 current.end().line(),
                                 current.end().column() + current.length(),
                             },
                         },
                         i18n::TranslationKey::
                             kDiagnosticParserConflictingStorageSpecifiers,
                         diagnostic::LabelMarkerType::kLine, {"mut", "const"}))
              .build());
    } else if (attribute.is_extern && attribute.is_static) [[unlikely]] {
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kUnexpectedToken)
                  .label(stream_->file_id(),
                         core::SourceRange{
                             core::SourceLocation{
                                 first_token.start().line(),
                                 first_token.start().column(),
                             },
                             core::SourceLocation{
                                 current.end().line(),
                                 current.end().column() + current.length(),
                             },
                         },
                         i18n::TranslationKey::
                             kDiagnosticParserConflictingStorageSpecifiers,
                         diagnostic::LabelMarkerType::kLine,
                         {"extern", "static"}))
              .build());
    }
  }

  switch (kind) {
    case Kind::kFunction: return parse_function_declaration(attribute);
    case Kind::kStruct: return parse_struct_declaration(attribute);
    case Kind::kEnumeration: return parse_enumeration_declaration(attribute);
    case Kind::kTrait: return parse_trait_declaration(attribute);
    case Kind::kImplementation: return parse_impl_declaration(attribute);
    case Kind::kUnion: return parse_union_declaration(attribute);
    case Kind::kModule: return parse_module_declaration(attribute);
    case Kind::kRedirect: return parse_redirect_declaration(attribute);
    case Kind::kIdentifier: return parse_assign_statement(attribute);
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
