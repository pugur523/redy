// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/stream.h"

#include <span>
#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "unicode/utf8/file_manager.h"

namespace unicode {

namespace {

Utf8FileManager manager;

Utf8Stream make_stream(std::u8string&& input) {
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(&manager, id);
  return stream;
}

}  // namespace

TEST(Utf8StreamTest, BasicAdvanceAndPeek) {
  Utf8Stream stream = make_stream(u8"abc\ndef");

  EXPECT_EQ(stream.peek(), 'a');
  stream.next();
  EXPECT_EQ(stream.peek(), 'b');
  stream.next();
  EXPECT_EQ(stream.line(), 1);
  EXPECT_EQ(stream.column(), 3);
  stream.next();  // 'c'
  stream.next();  // '\n'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 1);
  EXPECT_EQ(stream.peek(), 'd');
}

TEST(Utf8StreamTest, Utf8CodepointNextAndPeek) {
  Utf8Stream stream = make_stream(u8"aあ😊b");

  EXPECT_EQ(stream.peek(), 'a');
  EXPECT_EQ(stream.column(), 1);
  stream.next();

  // 'あ' (U+3042)
  EXPECT_EQ(stream.peek(), 0x3042u);
  EXPECT_EQ(stream.column(), 2);
  stream.next();

  // '😊' (U+1F60A)
  EXPECT_EQ(stream.peek(), 0x1F60Au);
  EXPECT_EQ(stream.column(), 3);
  stream.next();

  // 'b'
  EXPECT_EQ(stream.peek(), 'b');
  EXPECT_EQ(stream.column(), 4);
  stream.next();

  // eof
  EXPECT_TRUE(stream.eof());
}

TEST(Utf8StreamTest, Utf8PositionTracking) {
  Utf8Stream stream = make_stream(u8"a\nあ😊\nb");

  EXPECT_EQ(stream.line(), 1);
  EXPECT_EQ(stream.column(), 1);

  stream.next();  // 'a'
  EXPECT_EQ(stream.line(), 1);
  EXPECT_EQ(stream.column(), 2);

  stream.next();  // '\n'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 1);

  stream.next();  // 'あ'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 2);

  stream.next();  // '😊'
  EXPECT_EQ(stream.line(), 2);
  EXPECT_EQ(stream.column(), 3);

  stream.next();  // '\n'
  EXPECT_EQ(stream.line(), 3);
  EXPECT_EQ(stream.column(), 1);

  stream.next();  // 'b'
  EXPECT_EQ(stream.line(), 3);
  EXPECT_EQ(stream.column(), 2);
}

TEST(Utf8StreamTest, InvalidUtf8Handling) {
  // invalid utf8: 0xC0 0xAF (overlong encoding)
  std::u8string input = std::u8string(u8"a") + static_cast<char8_t>(0xC0) +
                        static_cast<char8_t>(0xAF) + u8"b";
  Utf8Stream stream = make_stream(std::move(input));

  EXPECT_EQ(stream.status(), Utf8Stream::Status::kInvalid);
  // invalid stream: do not use peek/next
  // EXPECT_EQ(stream.peek(), 'a');
  // stream.next();
  // should treat invalid byte sequence as individual bytes
  // EXPECT_NE(stream.peek(), 0x2F);
  // stream.next();
  // EXPECT_EQ(stream.peek(), 'b');
}

}  // namespace unicode
