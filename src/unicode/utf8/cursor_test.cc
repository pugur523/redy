// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/cursor.h"

#include <span>
#include <string>
#include <utility>

#include "gtest/gtest.h"
#include "unicode/utf8/file_manager.h"

namespace unicode {

namespace {

static Utf8FileManager manager;

Utf8Cursor make_cursor(std::u8string&& input) {
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Cursor cursor;
  cursor.init(manager.file(id));
  return cursor;
}

}  // namespace

TEST(Utf8CursorTest, BasicAdvanceAndPeek) {
  Utf8Cursor cursor = make_cursor(u8"abc\ndef");

  EXPECT_EQ(cursor.peek(), 'a');
  cursor.next();
  EXPECT_EQ(cursor.peek(), 'b');
  cursor.next();
  EXPECT_EQ(cursor.line(), 1);
  EXPECT_EQ(cursor.column(), 3);
  cursor.next();  // 'c'
  cursor.next();  // '\n'
  EXPECT_EQ(cursor.line(), 2);
  EXPECT_EQ(cursor.column(), 1);
  EXPECT_EQ(cursor.peek(), 'd');
}

TEST(Utf8CursorTest, Utf8CodepointNextAndPeek) {
  Utf8Cursor cursor = make_cursor(u8"aあ😊b");

  EXPECT_EQ(cursor.peek(), 'a');
  EXPECT_EQ(cursor.column(), 1);
  cursor.next();

  // 'あ' (U+3042)
  EXPECT_EQ(cursor.peek(), 0x3042u);
  EXPECT_EQ(cursor.column(), 2);
  cursor.next();

  // '😊' (U+1F60A)
  EXPECT_EQ(cursor.peek(), 0x1F60Au);
  EXPECT_EQ(cursor.column(), 3);
  cursor.next();

  // 'b'
  EXPECT_EQ(cursor.peek(), 'b');
  EXPECT_EQ(cursor.column(), 4);
  cursor.next();

  // eof
  EXPECT_TRUE(cursor.eof());
}

TEST(Utf8CursorTest, Utf8PositionTracking) {
  Utf8Cursor cursor = make_cursor(u8"a\nあ😊\nb");

  EXPECT_EQ(cursor.line(), 1);
  EXPECT_EQ(cursor.column(), 1);

  cursor.next();  // 'a'
  EXPECT_EQ(cursor.line(), 1);
  EXPECT_EQ(cursor.column(), 2);

  cursor.next();  // '\n'
  EXPECT_EQ(cursor.line(), 2);
  EXPECT_EQ(cursor.column(), 1);

  cursor.next();  // 'あ'
  EXPECT_EQ(cursor.line(), 2);
  EXPECT_EQ(cursor.column(), 2);

  cursor.next();  // '😊'
  EXPECT_EQ(cursor.line(), 2);
  EXPECT_EQ(cursor.column(), 3);

  cursor.next();  // '\n'
  EXPECT_EQ(cursor.line(), 3);
  EXPECT_EQ(cursor.column(), 1);

  cursor.next();  // 'b'
  EXPECT_EQ(cursor.line(), 3);
  EXPECT_EQ(cursor.column(), 2);
}

TEST(Utf8CursorTest, InvalidUtf8Handling) {
  // invalid utf8: 0xC0 0xAF (overlong encoding)
  std::u8string input = std::u8string(u8"a") + static_cast<char8_t>(0xC0) +
                        static_cast<char8_t>(0xAF) + u8"b";
  Utf8Cursor cursor = make_cursor(std::move(input));

  EXPECT_EQ(cursor.status(), Utf8Cursor::Status::kInvalid);
  // invalid cursor: do not use peek/next
  // EXPECT_EQ(cursor.peek(), 'a');
  // cursor.next();
  // should treat invalid byte sequence as individual bytes
  // EXPECT_NE(cursor.peek(), 0x2F);
  // cursor.next();
  // EXPECT_EQ(cursor.peek(), 'b');
}

}  // namespace unicode
