// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_TYPE_KEYWORD_H_
#define FRONTEND_BASE_KEYWORD_TYPE_KEYWORD_H_

#include <cstdint>

#include "core/check.h"

namespace base {

enum class TypeKeyword : uint8_t {
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
  kUsize = 12,
  kU128 = 13,
  kF32 = 14,
  kF64 = 15,
  kVoid = 16,
  kBool = 17,
  kChar = 18,
  kByte = 19,
};

inline const char* type_keyword_to_string(TypeKeyword keyword) {
  switch (keyword) {
    case TypeKeyword::kUnknown: return "unknown";
    case TypeKeyword::kAuto: return "auto";
    case TypeKeyword::kI8: return "i8";
    case TypeKeyword::kI16: return "i16";
    case TypeKeyword::kI32: return "i32";
    case TypeKeyword::kI64: return "i64";
    case TypeKeyword::kI128: return "i128";
    case TypeKeyword::kIsize: return "isize";
    case TypeKeyword::kU8: return "u8";
    case TypeKeyword::kU16: return "u16";
    case TypeKeyword::kU32: return "u32";
    case TypeKeyword::kU64: return "u64";
    case TypeKeyword::kU128: return "u128";
    case TypeKeyword::kUsize: return "usize";
    case TypeKeyword::kF32: return "f32";
    case TypeKeyword::kF64: return "f64";
    case TypeKeyword::kVoid: return "void";
    case TypeKeyword::kBool: return "bool";
    case TypeKeyword::kChar: return "char";
    case TypeKeyword::kByte: return "byte";
    default: DCHECK(false); return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_TYPE_KEYWORD_H_
