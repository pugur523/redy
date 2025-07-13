// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/token/token_stream.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "frontend/lexer/token/token.h"
#include "gtest/gtest.h"

namespace lexer {

TEST(TokenStreamTest, BasicNextAndPeek) {
  std::vector<Token> tokens;
  core::FileManager manager;
  core::FileId file_id = manager.add_virtual_file("x + 42");

  tokens.emplace_back(TokenKind::kIdentifier, &manager, file_id, 1, 1, 1);
  tokens.emplace_back(TokenKind::kPlus, &manager, file_id, 1, 3, 1);
  tokens.emplace_back(TokenKind::kLiteralNumber, &manager, file_id, 1, 5, 2);
  tokens.emplace_back(TokenKind::kEof, &manager, file_id, 1, 7, 0);

  TokenStream stream(std::move(tokens));

  EXPECT_EQ(stream.peek().kind(), TokenKind::kIdentifier);
  EXPECT_EQ(stream.peek().lexeme(), "x");

  EXPECT_EQ(stream.advance().kind(), TokenKind::kPlus);
  EXPECT_EQ(stream.peek().kind(), TokenKind::kPlus);

  EXPECT_EQ(stream.advance().kind(), TokenKind::kLiteralNumber);
  EXPECT_EQ(stream.peek().lexeme(), "42");

  EXPECT_EQ(stream.advance().kind(), TokenKind::kEof);
  EXPECT_TRUE(stream.eof());
}

TEST(TokenStreamTest, RewindWorks) {
  std::vector<Token> tokens;
  core::FileManager manager;
  core::FileId file_id = manager.add_virtual_file("foo 1");

  tokens.emplace_back(TokenKind::kIdentifier, &manager, file_id, 1, 1, 3);
  tokens.emplace_back(TokenKind::kLiteralNumber, &manager, file_id, 1, 5, 1);
  tokens.emplace_back(TokenKind::kEof, &manager, file_id, 1, 6, 0);

  TokenStream stream(std::move(tokens));

  stream.advance();  // foo
  std::size_t save_pos = stream.position();
  stream.advance();  // 1
  EXPECT_EQ(stream.peek().kind(), TokenKind::kEof);

  stream.rewind(save_pos);
  EXPECT_EQ(stream.peek().lexeme(), "1");
}

}  // namespace lexer
