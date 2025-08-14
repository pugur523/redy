// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<void> Parser::parse_declaration() {
  using Kind = base::TokenKind;

  Kind kind = peek().kind();

  // read builtin attribute
  ast::BuiltinAttribute attribute;
  while (!eof() && base::token_kind_is_attribute_keyword(kind)) {
    switch (kind) {
      case Kind::kMutable: attribute.is_mutable = true; break;
      case Kind::kConstant: attribute.is_const = true; break;
      case Kind::kExtern: attribute.is_extern = true; break;
      case Kind::kStatic: attribute.is_static = true; break;
      case Kind::kThreadLocal: attribute.is_thread_local = true; break;
      case Kind::kDeprecated: attribute.is_deprecated = true; break;
      case Kind::kPublic: attribute.is_public = true; break;
      default: break;
    }

    kind = next().kind();
  }

  if (has_any_builtin_attribute(attribute)) {
    if (attribute.is_mutable && attribute.is_const) [[unlikely]] {
      // return error
    } else if (attribute.is_extern && attribute.is_static) [[unlikely]] {
      // return error
    }
  }

  switch (kind) {
    case Kind::kFunction: parse_function_declaration(attribute);
    case Kind::kStruct: parse_struct_declaration(attribute);
    case Kind::kEnumeration: parse_enumeration_declaration(attribute);
    case Kind::kTrait: parse_trait_declaration(attribute);
    case Kind::kImplementation: parse_impl_declaration(attribute);
    case Kind::kUnion: parse_union_declaration(attribute);
    case Kind::kModule: parse_module_declaration(attribute);
    case Kind::kRedirect: parse_redirect_declaration(attribute);

    case Kind::kIdentifier: parse_assign_statement(attribute);

    default: break;  // return error
  }

  return ok<void>();
}

}  // namespace parser
