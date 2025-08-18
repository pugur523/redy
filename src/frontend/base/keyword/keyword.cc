// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/keyword.h"

#include <array>
#include <cstring>
#include <string>
#include <string_view>

#include "frontend/base/token/token.h"
#include "frontend/base/token/token_kind.h"

namespace base {

TokenKind lookup_id_or_keyword(std::u8string_view word) {
  switch (word.length()) {
    case 2:
      if (word == u8"i8") {
        return TokenKind::kI8;
      } else if (word == u8"u8") {
        return TokenKind::kU8;
      } else if (word == u8"if") {
        return TokenKind::kIf;
      } else if (word == u8"fn") {
        return TokenKind::kFunction;
      } else if (word == u8"as") {
        return TokenKind::kAs;
      } else {
        break;
      }

    case 3:
      if (word == u8"i16") {
        return TokenKind::kI16;
      } else if (word == u8"i32") {
        return TokenKind::kI32;
      } else if (word == u8"i64") {
        return TokenKind::kI64;
      } else if (word == u8"u16") {
        return TokenKind::kU16;
      } else if (word == u8"u32") {
        return TokenKind::kU32;
      } else if (word == u8"u64") {
        return TokenKind::kU64;
      } else if (word == u8"f32") {
        return TokenKind::kF32;
      } else if (word == u8"f64") {
        return TokenKind::kF64;
      } else if (word == u8"str") {
        return TokenKind::kStr;
      } else if (word == u8"for") {
        return TokenKind::kFor;
      } else if (word == u8"ret") {
        return TokenKind::kReturn;
      } else if (word == u8"mut") {
        return TokenKind::kMutable;
      } else if (word == u8"pub") {
        return TokenKind::kPublic;
      }
      break;

    case 4:
      if (word == u8"i128") {
        return TokenKind::kI128;
      } else if (word == u8"u128") {
        return TokenKind::kU128;
      } else if (word == u8"void") {
        return TokenKind::kVoid;
      } else if (word == u8"byte") {
        return TokenKind::kByte;
      } else if (word == u8"bool") {
        return TokenKind::kBool;
      } else if (word == u8"char") {
        return TokenKind::kChar;
      } else if (word == u8"else") {
        return TokenKind::kElse;
      } else if (word == u8"loop") {
        return TokenKind::kLoop;
      } else if (word == u8"enum") {
        return TokenKind::kEnumeration;
      } else if (word == u8"impl") {
        return TokenKind::kImplementation;
      } else if (word == u8"fast") {
        return TokenKind::kFast;
      } else if (word == u8"this") {
        return TokenKind::kThis;
      } else if (word == u8"true") {
        return TokenKind::kTrue;
      }
      break;

    case 5:
      if (word == u8"isize") {
        return TokenKind::kIsize;
      } else if (word == u8"usize") {
        return TokenKind::kUsize;
      } else if (word == u8"while") {
        return TokenKind::kWhile;
      } else if (word == u8"break") {
        return TokenKind::kBreak;
      } else if (word == u8"match") {
        return TokenKind::kMatch;
      } else if (word == u8"trait") {
        return TokenKind::kTrait;
      } else if (word == u8"union") {
        return TokenKind::kUnion;
      } else if (word == u8"const") {
        return TokenKind::kConstant;
      } else if (word == u8"async") {
        return TokenKind::kAsync;
      } else if (word == u8"await") {
        return TokenKind::kAwait;
      } else if (word == u8"false") {
        return TokenKind::kFalse;
      }

      break;

    case 6:
      if (word == u8"struct") {
        return TokenKind::kStruct;
      } else if (word == u8"module") {
        return TokenKind::kModule;
      } else if (word == u8"unsafe") {
        return TokenKind::kUnsafe;
      } else if (word == u8"extern") {
        return TokenKind::kExtern;
      } else if (word == u8"static") {
        return TokenKind::kStatic;
      }
      break;

    case 8:
      if (word == u8"continue") {
        return TokenKind::kContinue;
      } else if (word == u8"redirect") {
        return TokenKind::kRedirect;
      }
      break;

    case 12:
      if (word == u8"thread_local") {
        return TokenKind::kThreadLocal;
      }
      break;
  }

  return TokenKind::kIdentifier;
}

}  // namespace base
