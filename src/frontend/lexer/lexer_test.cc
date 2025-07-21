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
        fn main() -> void {
            world_str := "world";
            x: i32 = 42;
            print("hello {}", world_str);
            print("answer to the ultimate question of life, the universe, and everything is {}.", x);
            return;
        }
    )";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(source));
  Lexer lexer(&manager, id);

  while (true) {
    Token token = lexer.next_token().unwrap();

    EXPECT_FALSE(std::string(to_string(token.kind())).empty());

    EXPECT_GT(token.location().line(), 0);
    EXPECT_GT(token.location().column(), 0);

    EXPECT_NE(token.kind(), TokenKind::kUnknown);

    if (token.kind() == TokenKind::kEof) {
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
  core::FileId id = manager.add_virtual_file("x := 42;");
  Lexer lexer(&manager, id);

  Token t1 = lexer.next_token().unwrap();
  Token t2 = lexer.next_token().unwrap();
  Token t3 = lexer.next_token().unwrap();
  Token t4 = lexer.next_token().unwrap();
  Token t5 = lexer.next_token().unwrap();
  EXPECT_EQ(t1.kind(), TokenKind::kIdentifier);
  EXPECT_EQ(t2.kind(), TokenKind::kAssign);
  EXPECT_EQ(t3.kind(), TokenKind::kLiteralNumeric);
  EXPECT_EQ(t4.kind(), TokenKind::kSemicolon);
  EXPECT_EQ(t5.kind(), TokenKind::kEof);
}

TEST(LexerTest, HelloWorldFunction) {
  verify_hello_world();
}

}  // namespace lexer
