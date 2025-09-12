// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/resolver/symbol/symbol_table.h"

#include "frontend/base/string/string_interner.h"
#include "gtest/gtest.h"

namespace resolver {

namespace {

class SymbolTableTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  SymbolTable symbol_table_{&interner};

 private:
  base::StringInterner interner;
};

TEST_F(SymbolTableTest, PushAndPopScope) {
  // initially, there should be one scope.
  ASSERT_NO_FATAL_FAILURE(symbol_table_.push_scope());

  symbol_table_.declare_by_name("var1", hir::NodeId(1));
  EXPECT_EQ(symbol_table_.resolve_by_name("var1").id, hir::NodeId(1).id);

  symbol_table_.push_scope();
  symbol_table_.declare_by_name("var2", hir::NodeId(2));
  EXPECT_EQ(symbol_table_.resolve_by_name("var2").id, hir::NodeId(2).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("var1").id, hir::NodeId(1).id);

  symbol_table_.pop_scope();
  EXPECT_EQ(symbol_table_.resolve_by_name("var1").id, hir::NodeId(1).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("var2").id, hir::kInvalidNodeId.id);
}

TEST_F(SymbolTableTest, MultipleScopeLevels) {
  symbol_table_.push_scope();  // scope 1
  symbol_table_.declare_by_name("a", hir::NodeId(1));

  symbol_table_.push_scope();  // scope 2
  symbol_table_.declare_by_name("b", hir::NodeId(2));

  symbol_table_.push_scope();  // scope 3
  symbol_table_.declare_by_name("c", hir::NodeId(3));

  EXPECT_EQ(symbol_table_.resolve_by_name("a").id, hir::NodeId(1).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("b").id, hir::NodeId(2).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("c").id, hir::NodeId(3).id);

  symbol_table_.pop_scope();  // exit scope 3
  EXPECT_EQ(symbol_table_.resolve_by_name("a").id, hir::NodeId(1).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("b").id, hir::NodeId(2).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("c").id, hir::kInvalidNodeId.id);

  symbol_table_.pop_scope();  // exit scope 2
  EXPECT_EQ(symbol_table_.resolve_by_name("a").id, hir::NodeId(1).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("b").id, hir::kInvalidNodeId.id);
  EXPECT_EQ(symbol_table_.resolve_by_name("c").id, hir::kInvalidNodeId.id);
}

TEST_F(SymbolTableTest, VariableShadowing) {
  symbol_table_.push_scope();
  symbol_table_.declare_by_name("x", hir::NodeId(10));
  EXPECT_EQ(symbol_table_.resolve_by_name("x").id, hir::NodeId(10).id);

  symbol_table_.push_scope();
  // shadowing "x"
  symbol_table_.declare_by_name("x", hir::NodeId(20));
  EXPECT_EQ(symbol_table_.resolve_by_name("x").id, hir::NodeId(20).id);

  symbol_table_.pop_scope();

  // old "x" is restored
  EXPECT_EQ(symbol_table_.resolve_by_name("x").id, hir::NodeId(10).id);
}

TEST_F(SymbolTableTest, NestedShadowing) {
  symbol_table_.push_scope();
  symbol_table_.declare_by_name("y", hir::NodeId(100));

  symbol_table_.push_scope();
  symbol_table_.declare_by_name("y", hir::NodeId(200));

  symbol_table_.push_scope();
  symbol_table_.declare_by_name("y", hir::NodeId(300));
  EXPECT_EQ(symbol_table_.resolve_by_name("y").id, hir::NodeId(300).id);

  symbol_table_.pop_scope();
  EXPECT_EQ(symbol_table_.resolve_by_name("y").id, hir::NodeId(200).id);

  symbol_table_.pop_scope();
  EXPECT_EQ(symbol_table_.resolve_by_name("y").id, hir::NodeId(100).id);
}

TEST_F(SymbolTableTest, UndeclaredSymbolReturnsInvalidId) {
  symbol_table_.push_scope();
  symbol_table_.declare_by_name("test_var", hir::NodeId(99));
  EXPECT_EQ(symbol_table_.resolve_by_name("non_existent_var").id,
            hir::kInvalidNodeId.id);
}

TEST_F(SymbolTableTest, DeclaringMultipleSymbols) {
  symbol_table_.push_scope();
  symbol_table_.declare_by_name("a", hir::NodeId(1));
  symbol_table_.declare_by_name("b", hir::NodeId(2));
  symbol_table_.declare_by_name("c", hir::NodeId(3));

  EXPECT_EQ(symbol_table_.resolve_by_name("a").id, hir::NodeId(1).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("b").id, hir::NodeId(2).id);
  EXPECT_EQ(symbol_table_.resolve_by_name("c").id, hir::NodeId(3).id);
}

TEST_F(SymbolTableTest, EmptyTable) {
  // an empty symbol table should return an invalid id for any lookup.
  EXPECT_EQ(symbol_table_.resolve_by_name("anything").id,
            hir::kInvalidNodeId.id);
}

}  // namespace

}  // namespace resolver
