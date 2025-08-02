// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

#include <string>
#include <utility>

#include "gtest/gtest.h"

namespace lexer {

namespace {

void verify_hello_world() {
  std::string source = R"(
        fn main() -> i32 {
            world_str := "world"
            x: i32 = 42
            println#("hello {}", world_str)
            println#("answer to the ultimate question of life, the universe, and everything is {}.", x)
            ret 0
        }
    )";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(source));
  Lexer lexer(&manager, id);

  while (true) {
    base::Token token = lexer.next_token().unwrap();

    EXPECT_FALSE(std::string(token_kind_to_string(token.kind())).empty());

    EXPECT_GT(token.location().line(), 0);
    EXPECT_GT(token.location().column(), 0);

    EXPECT_NE(token.kind(), base::TokenKind::kUnknown);

    if (token.kind() == base::TokenKind::kEof) {
      EXPECT_TRUE(token.lexeme(&manager).empty());
      break;
    } else {
      EXPECT_FALSE(token.lexeme(&manager).empty());
    }
  }
}

}  // namespace

TEST(LexerTest, LexSimpleCode) {
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file("x := 42");
  Lexer lexer(&manager, id);

  base::Token t1 = lexer.next_token().unwrap();
  base::Token t2 = lexer.next_token().unwrap();
  base::Token t3 = lexer.next_token().unwrap();
  base::Token t4 = lexer.next_token().unwrap();
  EXPECT_EQ(t1.kind(), base::TokenKind::kIdentifier);
  EXPECT_EQ(t2.kind(), base::TokenKind::kAssign);
  EXPECT_EQ(t3.kind(), base::TokenKind::kLiteralNumeric);
  EXPECT_EQ(t4.kind(), base::TokenKind::kEof);
}

TEST(LexerTest, HelloWorldFunction) {
  verify_hello_world();
}

}  // namespace lexer
