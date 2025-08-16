// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/token/token.h"

#include "gtest/gtest.h"
#include "unicode/utf8/file.h"
#include "unicode/utf8/file_manager.h"

namespace base {

TEST(TokenTest, BasicConstruction) {
  unicode::Utf8FileManager manager;
  auto file_id = manager.register_virtual_file(u8"if");
  const unicode::Utf8File& file = manager.file(file_id);
  Token tok(TokenKind::kIf, core::SourceLocation(1, 1), 2);
  EXPECT_EQ(tok.kind(), TokenKind::kIf);
  EXPECT_EQ(tok.lexeme(file), "if");
  EXPECT_EQ(tok.start().line(), 1);
  EXPECT_EQ(tok.start().column(), 1);
  EXPECT_EQ(file_id, 0);
  EXPECT_FALSE(file.file_name().empty());

  EXPECT_FALSE(tok.dump(file).empty());
  EXPECT_FALSE(tok.dump_detailed(file).empty());
}

}  // namespace base
