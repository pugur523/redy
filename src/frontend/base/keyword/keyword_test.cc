// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/keyword.h"

#include "frontend/base/token/token_kind.h"
#include "gtest/gtest.h"

namespace base {

namespace {

void verify_keyword(std::u8string&& keyword, TokenKind kind) {
  EXPECT_EQ(lookup_id_or_keyword(keyword), kind);
}

}  // namespace

TEST(KeywordTest, LookupKeyword) {
  verify_keyword(u8"i8", TokenKind::kI8);
  verify_keyword(u8"i16", TokenKind::kI16);
  verify_keyword(u8"i32", TokenKind::kI32);
  verify_keyword(u8"i64", TokenKind::kI64);
  verify_keyword(u8"i128", TokenKind::kI128);
  verify_keyword(u8"isize", TokenKind::kIsize);
  verify_keyword(u8"u8", TokenKind::kU8);
  verify_keyword(u8"u16", TokenKind::kU16);
  verify_keyword(u8"u32", TokenKind::kU32);
  verify_keyword(u8"u64", TokenKind::kU64);
  verify_keyword(u8"u128", TokenKind::kU128);
  verify_keyword(u8"usize", TokenKind::kUsize);
  verify_keyword(u8"f32", TokenKind::kF32);
  verify_keyword(u8"f64", TokenKind::kF64);
  verify_keyword(u8"void", TokenKind::kVoid);
  verify_keyword(u8"byte", TokenKind::kByte);
  verify_keyword(u8"bool", TokenKind::kBool);
  verify_keyword(u8"char", TokenKind::kChar);
  verify_keyword(u8"str", TokenKind::kStr);

  verify_keyword(u8"if", TokenKind::kIf);
  verify_keyword(u8"else", TokenKind::kElse);
  verify_keyword(u8"loop", TokenKind::kLoop);
  verify_keyword(u8"while", TokenKind::kWhile);
  verify_keyword(u8"for", TokenKind::kFor);
  verify_keyword(u8"break", TokenKind::kBreak);
  verify_keyword(u8"continue", TokenKind::kContinue);
  verify_keyword(u8"ret", TokenKind::kReturn);
  verify_keyword(u8"match", TokenKind::kMatch);

  verify_keyword(u8"fn", TokenKind::kFunction);
  verify_keyword(u8"struct", TokenKind::kStruct);
  verify_keyword(u8"enum", TokenKind::kEnumeration);
  verify_keyword(u8"trait", TokenKind::kTrait);
  verify_keyword(u8"impl", TokenKind::kImplementation);
  verify_keyword(u8"union", TokenKind::kUnion);
  verify_keyword(u8"module", TokenKind::kModule);
  verify_keyword(u8"redirect", TokenKind::kRedirect);
  verify_keyword(u8"unsafe", TokenKind::kUnsafe);
  verify_keyword(u8"fast", TokenKind::kFast);
  verify_keyword(u8"use", TokenKind::kUse);

  verify_keyword(u8"mut", TokenKind::kMutable);
  verify_keyword(u8"const", TokenKind::kConstant);
  verify_keyword(u8"extern", TokenKind::kExtern);
  verify_keyword(u8"static", TokenKind::kStatic);
  verify_keyword(u8"thread_local", TokenKind::kThreadLocal);
  verify_keyword(u8"pub", TokenKind::kPublic);
  verify_keyword(u8"async", TokenKind::kAsync);

  verify_keyword(u8"as", TokenKind::kAs);
  verify_keyword(u8"this", TokenKind::kThis);
  verify_keyword(u8"await", TokenKind::kAwait);

  verify_keyword(u8"true", TokenKind::kTrue);
  verify_keyword(u8"false", TokenKind::kFalse);

  verify_keyword(u8"some_custom_id", TokenKind::kIdentifier);
  verify_keyword(u8"infer", TokenKind::kIdentifier);
}

}  // namespace base
