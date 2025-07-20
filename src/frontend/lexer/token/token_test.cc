// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/token/token.h"

#include "gtest/gtest.h"

namespace lexer {

TEST(TokenTest, BasicConstruction) {
  core::FileManager manager;
  auto file_id = manager.add_virtual_file("if");
  Token tok(TokenKind::kIf, core::SourceLocation(1, 1, file_id), 2);
  EXPECT_EQ(tok.kind(), TokenKind::kIf);
  EXPECT_EQ(tok.lexeme(&manager), "if");
  EXPECT_EQ(tok.location().line(), 1);
  EXPECT_EQ(tok.location().column(), 1);
  EXPECT_EQ(tok.location().file_id(), 0);
  EXPECT_FALSE(manager.file(tok.location().file_id()).file_name().empty());

  EXPECT_FALSE(tok.dump(&manager).empty());
  EXPECT_FALSE(tok.dump_detailed(&manager).empty());
}

}  // namespace lexer
