// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/string/string_interner.h"

#include <string>
#include <string_view>

#include "gtest/gtest.h"

namespace base {

class StringInternerTest : public testing::Test {
 protected:
  StringInterner interner;
};

TEST_F(StringInternerTest, InternsUniqueStrings) {
  std::string_view s1 = "hello";
  std::string_view s2 = "world";
  StringId id1 = interner.intern(s1);
  StringId id2 = interner.intern(s2);

  EXPECT_NE(id1, kInvalidStringId);
  EXPECT_NE(id2, kInvalidStringId);
  EXPECT_NE(id1, id2);
}

TEST_F(StringInternerTest, ReturnsSameIdForExistingString) {
  std::string_view s = "test_string";
  StringId id1 = interner.intern(s);
  StringId id2 = interner.intern(s);

  EXPECT_EQ(id1, id2);
}

TEST_F(StringInternerTest, LookupByIdReturnsCorrectString) {
  std::string_view s = "lookup_test";
  StringId id = interner.intern(s);

  EXPECT_EQ(interner.lookup(id), s);
}

TEST_F(StringInternerTest, LookupByStringReturnsCorrectId) {
  std::string_view s = "lookup_by_string_test";
  StringId original_id = interner.intern(s);

  EXPECT_EQ(interner.lookup(s), original_id);
  EXPECT_EQ(interner.lookup("non_existent_string"), kInvalidStringId);
}

TEST_F(StringInternerTest, HandlesEmptyString) {
  std::string_view empty_s = "";
  StringId id = interner.intern(empty_s);

  EXPECT_EQ(interner.lookup(id), empty_s);
  EXPECT_EQ(interner.lookup(empty_s), id);
}

TEST_F(StringInternerTest, HandlesLargeNumberOfStrings) {
  // check if rehash works
  for (int i = 0; i < 1000; ++i) {
    std::string s = "string_" + std::to_string(i);
    interner.intern(s);
  }

  std::string s500 = "string_500";
  StringId id500 = interner.lookup(s500);
  EXPECT_NE(id500, kInvalidStringId);
  EXPECT_EQ(interner.lookup(id500), s500);
}

}  // namespace base
