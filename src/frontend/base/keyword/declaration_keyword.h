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
  kFunction = 1,        // fn
  kStruct = 2,          // struct
  kEnumeration = 3,     // enum
  kTrait = 4,           // trait
  kImplementation = 5,  // impl
  kUnion = 6,           // union
  kModule = 7,          // module
  kRedirect = 8,        // redirect
  kUnsafe = 9,          // unsafe
  kFast = 10,           // fast
  kUse = 11,            // use
};

inline DeclarationKeyword token_kind_to_declaration_keyword(TokenKind kind) {
  switch (kind) {
    case TokenKind::kFunction: return DeclarationKeyword::kFunction;
    case TokenKind::kStruct: return DeclarationKeyword::kStruct;
    case TokenKind::kEnumeration: return DeclarationKeyword::kEnumeration;
    case TokenKind::kTrait: return DeclarationKeyword::kTrait;
    case TokenKind::kImplementation: return DeclarationKeyword::kImplementation;
    case TokenKind::kUnion: return DeclarationKeyword::kUnion;
    case TokenKind::kModule: return DeclarationKeyword::kModule;
    case TokenKind::kRedirect: return DeclarationKeyword::kRedirect;
    case TokenKind::kUnsafe: return DeclarationKeyword::kUnsafe;
    case TokenKind::kFast: return DeclarationKeyword::kFast;
    case TokenKind::kUse: return DeclarationKeyword::kUse;
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
    case DeclarationKeyword::kFunction: return "function";
    case DeclarationKeyword::kStruct: return "struct";
    case DeclarationKeyword::kEnumeration: return "enumeration";
    case DeclarationKeyword::kTrait: return "trait";
    case DeclarationKeyword::kImplementation: return "implementation";
    case DeclarationKeyword::kUnion: return "union";
    case DeclarationKeyword::kModule: return "module";
    case DeclarationKeyword::kRedirect: return "redirect";
    case DeclarationKeyword::kUnsafe: return "unsafe";
    case DeclarationKeyword::kFast: return "fast";
    case DeclarationKeyword::kUse: return "use";
    default: return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_DECLARATION_KEYWORD_H_
