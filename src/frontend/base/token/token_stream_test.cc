// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/token/token_stream.h"

#include <cstddef>
#include <utility>
#include <vector>

#include "frontend/base/token/token.h"
#include "gtest/gtest.h"
#include "unicode/utf8/file_manager.h"

namespace base {

TEST(TokenStreamTest, BasicNextAndPeek) {
  std::vector<Token> tokens;
  unicode::Utf8FileManager manager;
  unicode::Utf8FileId file_id = manager.add_virtual_file(u8"x + 42");
  const unicode::Utf8File& file = manager.file(file_id);

  tokens.emplace_back(TokenKind::kIdentifier, 1, 1, 1);
  tokens.emplace_back(TokenKind::kPlus, 1, 3, 1);
  tokens.emplace_back(TokenKind::kLiteralNumeric, 1, 5, 2);
  tokens.emplace_back(TokenKind::kEof, 1, 7, 0);

  TokenStream stream(std::move(tokens), file);

  EXPECT_EQ(stream.peek().kind(), TokenKind::kIdentifier);
  EXPECT_EQ(stream.peek().lexeme(file), "x");

  EXPECT_EQ(stream.next().kind(), TokenKind::kPlus);
  EXPECT_EQ(stream.peek().kind(), TokenKind::kPlus);

  EXPECT_EQ(stream.next().kind(), TokenKind::kLiteralNumeric);
  EXPECT_EQ(stream.peek().lexeme(file), "42");

  EXPECT_EQ(stream.next().kind(), TokenKind::kEof);
  EXPECT_TRUE(stream.eof());
}

TEST(TokenStreamTest, RewindWorks) {
  std::vector<Token> tokens;
  unicode::Utf8FileManager manager;
  unicode::Utf8FileId file_id = manager.add_virtual_file(u8"foo 1");
  const unicode::Utf8File& file = manager.file(file_id);

  tokens.emplace_back(TokenKind::kIdentifier, 1, 1, 3);
  tokens.emplace_back(TokenKind::kLiteralNumeric, 1, 5, 1);
  tokens.emplace_back(TokenKind::kEof, 1, 6, 0);

  TokenStream stream(std::move(tokens), file);

  stream.next();  // foo
  std::size_t save_pos = stream.position();
  stream.next();  // 1
  EXPECT_EQ(stream.peek().kind(), TokenKind::kEof);

  stream.rewind(save_pos);
  EXPECT_EQ(stream.peek().lexeme(file), "1");
}

}  // namespace base
