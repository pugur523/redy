// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_TYPE_H_
#define FRONTEND_BASE_KEYWORD_TYPE_H_

#include <cstdint>

#include "core/check.h"
#include "frontend/base/token/token_kind.h"

namespace base {

enum class TypeKind : uint8_t {
  kUnknown = 0,
  kAuto = 1,
  kI8 = 2,
  kI16 = 3,
  kI32 = 4,
  kI64 = 5,
  kI128 = 6,
  kIsize = 7,
  kU8 = 8,
  kU16 = 9,
  kU32 = 10,
  kU64 = 11,
  kU128 = 12,
  kUsize = 13,
  kF32 = 14,
  kF64 = 15,
  kVoid = 16,
  kByte = 17,
  kBool = 18,
  kChar = 19,
  kStr = 20,
  kUserDefined = 22,
};

inline bool token_kind_is_primitive_type(TokenKind kind) {
  return TokenKind::kI8 <= kind && kind <= TokenKind::kStr;
}

inline TypeKind token_kind_to_type_kind(TokenKind kind) {
  switch (kind) {
    case TokenKind::kI8: return TypeKind::kI8;
    case TokenKind::kI16: return TypeKind::kI16;
    case TokenKind::kI32: return TypeKind::kI32;
    case TokenKind::kI64: return TypeKind::kI64;
    case TokenKind::kI128: return TypeKind::kI128;
    case TokenKind::kIsize: return TypeKind::kIsize;
    case TokenKind::kU8: return TypeKind::kU8;
    case TokenKind::kU16: return TypeKind::kU16;
    case TokenKind::kU32: return TypeKind::kU32;
    case TokenKind::kU64: return TypeKind::kU64;
    case TokenKind::kU128: return TypeKind::kU128;
    case TokenKind::kUsize: return TypeKind::kUsize;
    case TokenKind::kF32: return TypeKind::kF32;
    case TokenKind::kF64: return TypeKind::kF64;
    case TokenKind::kVoid: return TypeKind::kVoid;
    case TokenKind::kByte: return TypeKind::kByte;
    case TokenKind::kBool: return TypeKind::kBool;
    case TokenKind::kChar: return TypeKind::kChar;
    case TokenKind::kStr: return TypeKind::kStr;
    default: DCHECK(false); return TypeKind::kUnknown;
  }
}

inline const char* type_kind_to_string(TypeKind kind) {
  switch (kind) {
    case TypeKind::kUnknown: return "unknown";
    case TypeKind::kAuto: return "auto";
    case TypeKind::kI8: return "i8";
    case TypeKind::kI16: return "i16";
    case TypeKind::kI32: return "i32";
    case TypeKind::kI64: return "i64";
    case TypeKind::kI128: return "i128";
    case TypeKind::kIsize: return "isize";
    case TypeKind::kU8: return "u8";
    case TypeKind::kU16: return "u16";
    case TypeKind::kU32: return "u32";
    case TypeKind::kU64: return "u64";
    case TypeKind::kU128: return "u128";
    case TypeKind::kUsize: return "usize";
    case TypeKind::kF32: return "f32";
    case TypeKind::kF64: return "f64";
    case TypeKind::kVoid: return "void";
    case TypeKind::kByte: return "byte";
    case TypeKind::kBool: return "bool";
    case TypeKind::kChar: return "char";
    case TypeKind::kStr: return "str";
    case TypeKind::kUserDefined: return "user defined";
    default: DCHECK(false); return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_TYPE_H_
