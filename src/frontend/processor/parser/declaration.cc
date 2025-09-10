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
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_decl_stmt() {
  auto storage_attr_r = parse_storage_attributes();
  if (storage_attr_r.is_err()) {
    return err<NodeId>(std::move(storage_attr_r).unwrap_err());
  }
  const ast::StorageAttributeData attribute =
      std::move(storage_attr_r).unwrap();

  using Kind = base::TokenKind;
  const Kind kind = peek().kind();

  // local macro, undef at the bottom of this file
#define TRY_RETURN_DECL(fn, node_kind)                 \
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
      TRY_RETURN_DECL(parse_function_decl_stmt,
                      ast::NodeKind::kFunctionDeclaration);
    }
    case Kind::kStruct: {
      TRY_RETURN_DECL(parse_struct_decl_stmt,
                      ast::NodeKind::kStructDeclaration);
    }
    case Kind::kEnumeration: {
      TRY_RETURN_DECL(parse_enumeration_decl_stmt,
                      ast::NodeKind::kEnumDeclaration);
    }
    case Kind::kTrait: {
      TRY_RETURN_DECL(parse_trait_decl_stmt, ast::NodeKind::kTraitDeclaration);
    }
    case Kind::kImplementation: {
      TRY_RETURN_DECL(parse_impl_decl_stmt, ast::NodeKind::kImplDeclaration);
    }
    case Kind::kUnion: {
      TRY_RETURN_DECL(parse_union_decl_stmt, ast::NodeKind::kUnionDeclaration);
    }
    case Kind::kModule: {
      TRY_RETURN_DECL(parse_module_decl_stmt,
                      ast::NodeKind::kModuleDeclaration);
    }
    case Kind::kRedirect: {
      TRY_RETURN_DECL(parse_redirect_decl_stmt,
                      ast::NodeKind::kRedirectDeclaration);
    }
    case Kind::kIdentifier: {
      TRY_RETURN_DECL(parse_assign_stmt, ast::NodeKind::kAssignStatement);
    }
#undef TRY_RETURN_DECL
    default: {
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kUnexpectedToken)
                  .label(
                      stream_->file_id(), peek().range(),
                      i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                      diagnostic::LabelMarkerType::kLine,
                      {translator_->translate(
                           i18n::TranslationKey::kTermDeclaration),
                       translator_->translate(
                           base::token_kind_to_tr_key(kind))}))
              .build());
    }
  }
}

}  // namespace parser
