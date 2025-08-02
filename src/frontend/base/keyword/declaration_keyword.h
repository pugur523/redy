// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_DECLARATION_KEYWORD_H_
#define FRONTEND_BASE_KEYWORD_DECLARATION_KEYWORD_H_

#include <cstdint>

#include "frontend/base/token/token_kind.h"

namespace base {

enum class DeclarationKeyword : uint8_t {
  kUnknown = 0,
  kMutable = 1,         // mut
  kFunction = 2,        // fn
  kStruct = 3,          // struct
  kEnumeration = 4,     // enum
  kTrait = 5,           // trait
  kImplementation = 6,  // impl
  kUnion = 7,           // union
  kRedirect = 8,        // redirect
  kConstant = 9,        // const
  kExtern = 10,         // extern
  kStatic = 11,         // static
  kThreadLocal = 12,    // thread_local
};

inline DeclarationKeyword token_kind_to_declaration_keyword(TokenKind kind) {
  switch (kind) {
    case TokenKind::kMutable: return DeclarationKeyword::kMutable;
    case TokenKind::kFunction: return DeclarationKeyword::kFunction;
    case TokenKind::kStruct: return DeclarationKeyword::kStruct;
    case TokenKind::kEnumeration: return DeclarationKeyword::kEnumeration;
    case TokenKind::kTrait: return DeclarationKeyword::kTrait;
    case TokenKind::kImplementation: return DeclarationKeyword::kImplementation;
    case TokenKind::kUnion: return DeclarationKeyword::kUnion;
    case TokenKind::kRedirect: return DeclarationKeyword::kRedirect;
    case TokenKind::kConstant: return DeclarationKeyword::kConstant;
    case TokenKind::kExtern: return DeclarationKeyword::kExtern;
    case TokenKind::kStatic: return DeclarationKeyword::kStatic;
    case TokenKind::kThreadLocal: return DeclarationKeyword::kThreadLocal;
    default: return DeclarationKeyword::kUnknown;
  }
}

inline bool token_kind_is_declaration_keyword(TokenKind kind) {
  return token_kind_to_declaration_keyword(kind) !=
         DeclarationKeyword::kUnknown;
}

inline const char* declaration_keyword_to_string(DeclarationKeyword keyword) {
  switch (keyword) {
    case DeclarationKeyword::kUnknown: return "unknown";
    case DeclarationKeyword::kMutable: return "mutable";
    case DeclarationKeyword::kFunction: return "function";
    case DeclarationKeyword::kStruct: return "struct";
    case DeclarationKeyword::kEnumeration: return "enumeration";
    case DeclarationKeyword::kTrait: return "trait";
    case DeclarationKeyword::kImplementation: return "implementation";
    case DeclarationKeyword::kUnion: return "union";
    case DeclarationKeyword::kRedirect: return "redirect";
    case DeclarationKeyword::kConstant: return "constant";
    case DeclarationKeyword::kExtern: return "extern";
    case DeclarationKeyword::kStatic: return "static";
    case DeclarationKeyword::kThreadLocal: return "thread local";
    default: return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_DECLARATION_KEYWORD_H_
