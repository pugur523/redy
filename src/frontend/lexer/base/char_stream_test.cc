// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/base/char_stream.h"

#include <string>
#include <utility>

#include "core/base/file_manager.h"
#include "gtest/gtest.h"

namespace lexer {

TEST(CharStreamTest, BasicAdvanceAndPeek) {
  std::string input("abc\ndef");
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager, id);

  EXPECT_EQ(cs.peek(), 'a');
  cs.advance();
  EXPECT_EQ(cs.peek(), 'b');
  cs.advance();
  EXPECT_EQ(cs.line(), 1);
  EXPECT_EQ(cs.column(), 3);
  cs.advance();  // 'c'
  cs.advance();  // '\n'
  EXPECT_EQ(cs.line(), 2);
  EXPECT_EQ(cs.column(), 1);
}

}  // namespace lexer
