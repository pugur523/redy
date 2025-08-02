// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/keyword.h"

#include "frontend/base/token/token.h"
#include "gtest/gtest.h"

namespace base {

TEST(KeywordTest, LookupKeyword) {
  EXPECT_EQ(lookup_id_or_keyword("if"), TokenKind::kIf);
  EXPECT_EQ(lookup_id_or_keyword("else"), TokenKind::kElse);
  EXPECT_EQ(lookup_id_or_keyword("while"), TokenKind::kWhile);
  EXPECT_EQ(lookup_id_or_keyword("custom_id"), TokenKind::kIdentifier);
}

}  // namespace base
