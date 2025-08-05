// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/data/char_stream.h"

#include <string>
#include <utility>

#include "core/base/file_manager.h"
#include "gtest/gtest.h"

namespace base {

TEST(CharStreamTest, BasicAdvanceAndPeek) {
  std::string input("abc\ndef");
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager.file(id));

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

TEST(CharStreamTest, Utf8CodepointAdvanceAndPeek) {
  std::string input = "aあ😊b";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager.file(id));

  EXPECT_EQ(cs.peek(), 'a');
  EXPECT_EQ(cs.peek_codepoint(), 'a');
  EXPECT_EQ(cs.codepoint_byte_length(), 1u);
  EXPECT_EQ(cs.codepoint_column(), 1u);
  cs.advance_codepoint();

  // 'あ' (U+3042)
  EXPECT_EQ(cs.peek(), static_cast<char>(0xE3));
  EXPECT_EQ(cs.peek_codepoint(), 0x3042u);
  EXPECT_EQ(cs.codepoint_byte_length(), 3u);
  EXPECT_EQ(cs.codepoint_column(), 2u);
  cs.advance_codepoint();

  // '😊' (U+1F60A)
  EXPECT_EQ(cs.peek_codepoint(), 0x1F60Au);
  EXPECT_EQ(cs.codepoint_byte_length(), 4u);
  EXPECT_EQ(cs.codepoint_column(), 3u);
  cs.advance_codepoint();

  // 'b'
  EXPECT_EQ(cs.peek_codepoint(), 'b');
  EXPECT_EQ(cs.codepoint_column(), 4u);
  cs.advance_codepoint();

  // eof
  EXPECT_TRUE(cs.eof());
}

TEST(CharStreamTest, Utf8PositionTracking) {
  std::string input = "a\nあ😊\nb";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager.file(id));

  EXPECT_EQ(cs.line(), 1u);
  EXPECT_EQ(cs.column(), 1u);
  EXPECT_EQ(cs.codepoint_column(), 1u);
  cs.advance_codepoint();  // 'a'

  cs.advance_codepoint();  // '\n'
  EXPECT_EQ(cs.line(), 2u);
  EXPECT_EQ(cs.column(), 1u);
  EXPECT_EQ(cs.codepoint_column(), 1u);

  cs.advance_codepoint();  // 'あ'
  EXPECT_EQ(cs.column(), 4u);
  EXPECT_EQ(cs.codepoint_column(), 2u);

  cs.advance_codepoint();  // '😊'
  EXPECT_EQ(cs.column(), 8u);
  EXPECT_EQ(cs.codepoint_column(), 3u);

  cs.advance_codepoint();  // '\n'
  EXPECT_EQ(cs.line(), 3u);
  EXPECT_EQ(cs.column(), 1u);
  EXPECT_EQ(cs.codepoint_column(), 1u);

  cs.advance_codepoint();  // 'b'
  EXPECT_EQ(cs.line(), 3u);
  EXPECT_EQ(cs.codepoint_column(), 2u);
}

TEST(CharStreamTest, InvalidUtf8Handling) {
  // invalid utf8: 0xC0 0xAF (overlong encoding)
  std::string input = std::string("a") + static_cast<char>(0xC0) +
                      static_cast<char>(0xAF) + "b";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager.file(id));

  EXPECT_FALSE(cs.is_valid_utf8_whole_stream());
  EXPECT_EQ(cs.peek_codepoint(), 'a');
  cs.advance_codepoint();
  // should treat invalid byte sequence as individual bytes
  EXPECT_NE(cs.peek_codepoint(), 0x2F);
  cs.advance_codepoint();
  EXPECT_EQ(cs.peek_codepoint(), 'b');
}

}  // namespace base
