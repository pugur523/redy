// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_TYPE_H_
#define FRONTEND_BASE_KEYWORD_TYPE_H_

#include <cstdint>

#include "core/check.h"
#include "frontend/base/token/token_kind.h"

namespace base {

enum class TypeCategory : uint8_t {
  kUnknown = 0,
  kPrimitive = 1,
  kArray = 2,
  kUserDefined = 3,
};

enum class PrimitiveType : uint8_t {
  kUnknown = 0,
  kI8 = 1,
  kI16 = 2,
  kI32 = 3,
  kI64 = 4,
  kI128 = 5,
  kIsize = 6,
  kU8 = 7,
  kU16 = 8,
  kU32 = 9,
  kU64 = 10,
  kU128 = 11,
  kUsize = 12,
  kF32 = 13,
  kF64 = 14,
  kVoid = 15,
  kByte = 16,
  kBool = 17,
  kChar = 18,
  kStr = 19,
};

inline bool token_kind_is_primitive_type(TokenKind kind) {
  return TokenKind::kI8 <= kind && kind <= TokenKind::kStr;
}

inline PrimitiveType token_kind_to_primitive_type(TokenKind kind) {
  switch (kind) {
    case TokenKind::kI8: return PrimitiveType::kI8;
    case TokenKind::kI16: return PrimitiveType::kI16;
    case TokenKind::kI32: return PrimitiveType::kI32;
    case TokenKind::kI64: return PrimitiveType::kI64;
    case TokenKind::kI128: return PrimitiveType::kI128;
    case TokenKind::kIsize: return PrimitiveType::kIsize;
    case TokenKind::kU8: return PrimitiveType::kU8;
    case TokenKind::kU16: return PrimitiveType::kU16;
    case TokenKind::kU32: return PrimitiveType::kU32;
    case TokenKind::kU64: return PrimitiveType::kU64;
    case TokenKind::kU128: return PrimitiveType::kU128;
    case TokenKind::kUsize: return PrimitiveType::kUsize;
    case TokenKind::kF32: return PrimitiveType::kF32;
    case TokenKind::kF64: return PrimitiveType::kF64;
    case TokenKind::kVoid: return PrimitiveType::kVoid;
    case TokenKind::kByte: return PrimitiveType::kByte;
    case TokenKind::kBool: return PrimitiveType::kBool;
    case TokenKind::kChar: return PrimitiveType::kChar;
    case TokenKind::kStr: return PrimitiveType::kStr;
    default: DCHECK(false); return PrimitiveType::kUnknown;
  }
}

inline const char* type_kind_to_string(PrimitiveType kind) {
  switch (kind) {
    case PrimitiveType::kUnknown: return "unknown";
    case PrimitiveType::kI8: return "i8";
    case PrimitiveType::kI16: return "i16";
    case PrimitiveType::kI32: return "i32";
    case PrimitiveType::kI64: return "i64";
    case PrimitiveType::kI128: return "i128";
    case PrimitiveType::kIsize: return "isize";
    case PrimitiveType::kU8: return "u8";
    case PrimitiveType::kU16: return "u16";
    case PrimitiveType::kU32: return "u32";
    case PrimitiveType::kU64: return "u64";
    case PrimitiveType::kU128: return "u128";
    case PrimitiveType::kUsize: return "usize";
    case PrimitiveType::kF32: return "f32";
    case PrimitiveType::kF64: return "f64";
    case PrimitiveType::kVoid: return "void";
    case PrimitiveType::kByte: return "byte";
    case PrimitiveType::kBool: return "bool";
    case PrimitiveType::kChar: return "char";
    case PrimitiveType::kStr: return "str";
    default: DCHECK(false); return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_TYPE_H_
