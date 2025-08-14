// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/keyword.h"

#include "frontend/base/token/token.h"
#include "gtest/gtest.h"

namespace base {

TEST(KeywordTest, LookupKeyword) {
  EXPECT_EQ(lookup_id_or_keyword(u8"if"), TokenKind::kIf);
  EXPECT_EQ(lookup_id_or_keyword(u8"else"), TokenKind::kElse);
  EXPECT_EQ(lookup_id_or_keyword(u8"while"), TokenKind::kWhile);
  EXPECT_EQ(lookup_id_or_keyword(u8"custom_id"), TokenKind::kIdentifier);
  EXPECT_EQ(lookup_id_or_keyword(u8"i32"), TokenKind::kI32);
  EXPECT_EQ(lookup_id_or_keyword(u8"infer"), TokenKind::kIdentifier);
}

}  // namespace base
