// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/base/utf8_stream.h"

#include <span>
#include <string>
#include <utility>

#include "core/base/file_manager.h"
#include "gtest/gtest.h"

namespace unicode {

namespace {

static core::FileManager manager;

Utf8Stream make_stream(std::string&& input) {
  core::FileId id = manager.add_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(manager.file(id));
  return stream;
}

}  // namespace

TEST(Utf8StreamTest, BasicAdvanceAndPeek) {
  Utf8Stream stream = make_stream("abc\ndef");

  EXPECT_EQ(stream.peek(), 'a');
  stream.advance();
  EXPECT_EQ(stream.peek(), 'b');
  stream.advance();
  EXPECT_EQ(stream.line(), 1);
  EXPECT_EQ(stream.column(), 3);
  stream.advance();  // 'c'
  stream.advance();  // '\n'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 1);
  EXPECT_EQ(stream.peek(), 'd');
}

TEST(Utf8StreamTest, Utf8CodepointAdvanceAndPeek) {
  Utf8Stream stream = make_stream("aあ😊b");

  EXPECT_EQ(stream.peek(), 'a');
  EXPECT_EQ(stream.column(), 1);
  stream.advance();

  // 'あ' (U+3042)
  EXPECT_EQ(stream.peek(), 0x3042u);
  EXPECT_EQ(stream.column(), 2);
  stream.advance();

  // '😊' (U+1F60A)
  EXPECT_EQ(stream.peek(), 0x1F60Au);
  EXPECT_EQ(stream.column(), 3);
  stream.advance();

  // 'b'
  EXPECT_EQ(stream.peek(), 'b');
  EXPECT_EQ(stream.column(), 4);
  stream.advance();

  // eof
  EXPECT_TRUE(stream.eof());
}

TEST(Utf8StreamTest, Utf8PositionTracking) {
  Utf8Stream stream = make_stream("a\nあ😊\nb");

  EXPECT_EQ(stream.line(), 1);
  EXPECT_EQ(stream.column(), 1);

  stream.advance();  // 'a'
  EXPECT_EQ(stream.line(), 1);
  EXPECT_EQ(stream.column(), 2);

  stream.advance();  // '\n'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 1);

  stream.advance();  // 'あ'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 2);

  stream.advance();  // '😊'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 3);

  stream.advance();  // '\n'
  EXPECT_EQ(stream.line(), 3);
  EXPECT_EQ(stream.column(), 1);

  stream.advance();  // 'b'
  EXPECT_EQ(stream.line(), 3);
  EXPECT_EQ(stream.column(), 2);
}

TEST(Utf8StreamTest, InvalidUtf8Handling) {
  // invalid utf8: 0xC0 0xAF (overlong encoding)
  std::string input = std::string("a") + static_cast<char>(0xC0) +
                      static_cast<char>(0xAF) + "b";
  Utf8Stream stream = make_stream(std::move(input));

  EXPECT_FALSE(stream.valid());
  // invalid stream: do not use peek/advance
  // EXPECT_EQ(stream.peek(), 'a');
  // stream.advance();
  // should treat invalid byte sequence as individual bytes
  // EXPECT_NE(stream.peek(), 0x2F);
  // stream.advance();
  // EXPECT_EQ(stream.peek(), 'b');
}

}  // namespace unicode
