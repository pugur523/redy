// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "core/base/source_location.h"
#include "frontend/ast/base/base_node.h"
#include "frontend/ast/nodes/node_util.h"
#include "frontend/lexer/token/token_kind.h"
#include "gtest/gtest.h"

using Token = lexer::Token;
using TokenKind = lexer::TokenKind;

namespace ast {

namespace {

static core::FileManager file_manager;

}

Token make_dummy_token(std::string&& lexeme = "dummy",
                       TokenKind kind = TokenKind::kIdentifier) {
  std::size_t len = lexeme.length();
  core::FileId id = file_manager.add_virtual_file(std::move(lexeme));
  return Token(kind, core::SourceLocation(1, 1, id), len);
}

TEST(ASTNodeTest, IdentifierNodeTest) {
  Token tok = make_dummy_token("foo");
  IdentifierNode node(tok, "foo");

  EXPECT_EQ(node.token.lexeme(&file_manager), "foo");
  EXPECT_EQ(node.name, "foo");
}

TEST(ASTNodeTest, LiteralNodeIntegerTest) {
  Token tok = make_dummy_token("42", TokenKind::kLiteralNumeric);
  LiteralNode node(tok, LiteralNode::Type::kNumeric, "42");

  EXPECT_EQ(node.token.lexeme(&file_manager), "42");
  EXPECT_EQ(node.type, LiteralNode::Type::kNumeric);
  EXPECT_EQ(node.value, "42");
}

TEST(ASTNodeTest, LiteralNodeBooleanTest) {
  Token tok = make_dummy_token("true", TokenKind::kTrue);
  LiteralNode node(tok, true);

  EXPECT_EQ(node.type, LiteralNode::Type::kBoolean);
  EXPECT_EQ(node.value, "true");
}

TEST(ASTNodeTest, BinaryOpNodeTest) {
  Token tok = make_dummy_token("+", TokenKind::kPlus);

  auto left =
      make_node<LiteralNode>(make_dummy_token("1", TokenKind::kLiteralNumeric),
                             LiteralNode::Type::kNumeric, "1");
  auto right =
      make_node<LiteralNode>(make_dummy_token("2", TokenKind::kLiteralNumeric),
                             LiteralNode::Type::kNumeric, "2");

  BinaryOpNode node(tok, BinaryOpNode::Operator::kAdd, std::move(left),
                    std::move(right));

  EXPECT_EQ(node.op, BinaryOpNode::Operator::kAdd);
  EXPECT_TRUE(std::holds_alternative<std::unique_ptr<LiteralNode>>(node.left));
  EXPECT_TRUE(std::holds_alternative<std::unique_ptr<LiteralNode>>(node.right));
}

TEST(ASTNodeTest, UnaryOpNodeTest) {
  Token tok = make_dummy_token("-", TokenKind::kMinus);
  auto operand =
      make_node<LiteralNode>(make_dummy_token("1", TokenKind::kLiteralNumeric),
                             LiteralNode::Type::kNumeric, "1");

  UnaryOpNode node(tok, UnaryOpNode::Operator::kNegate, std::move(operand));

  EXPECT_EQ(node.op, UnaryOpNode::Operator::kNegate);
  EXPECT_TRUE(
      std::holds_alternative<std::unique_ptr<LiteralNode>>(node.operand));
}

TEST(ASTNodeTest, TypeNodeTest) {
  Token tok = make_dummy_token("i32", TokenKind::kIdentifier);
  TypeNode node(tok, "i32");

  EXPECT_EQ(node.type_name, "i32");
}

TEST(ASTNodeTest, VariableDeclarationNodeTest) {
  Token tok = make_dummy_token("x", TokenKind::kIdentifier);

  auto type = make_node<TypeNode>(make_dummy_token("i32"), "i32");
  auto init = make_node<LiteralNode>(make_dummy_token("0"),
                                     LiteralNode::Type::kNumeric, "0");

  VariableDeclarationNode node(tok, "x", false, std::move(type),
                               std::move(init));

  EXPECT_EQ(node.name, "x");
  EXPECT_TRUE(node.type.has_value());
  EXPECT_TRUE(node.initializer.has_value());
}

TEST(ASTNodeTest, ReturnNodeEmptyTest) {
  Token tok = make_dummy_token("return", TokenKind::kReturn);
  ReturnNode node(tok);

  EXPECT_FALSE(node.value.has_value());
}

TEST(ASTNodeTest, ReturnNodeWithValueTest) {
  Token tok = make_dummy_token("return", TokenKind::kReturn);
  auto val = make_node<LiteralNode>(make_dummy_token("42"),
                                    LiteralNode::Type::kNumeric, "42");
  ReturnNode node(tok, std::move(val));

  EXPECT_TRUE(node.value.has_value());
  EXPECT_TRUE(
      std::holds_alternative<std::unique_ptr<LiteralNode>>(*node.value));
}

TEST(ASTNodeTest, BlockNodeTest) {
  Token tok = make_dummy_token("{", TokenKind::kLBrace);

  std::vector<ASTNode> stmts;
  stmts.push_back(make_node<ReturnNode>(
      make_dummy_token("return"),
      make_node<LiteralNode>(make_dummy_token("1"), LiteralNode::Type::kNumeric,
                             "1")));

  BlockNode node(tok, std::move(stmts));

  EXPECT_EQ(node.statements.size(), 1);
  EXPECT_TRUE(
      std::holds_alternative<std::unique_ptr<ReturnNode>>(node.statements[0]));
}

TEST(ASTNodeTest, ParameterNodeTest) {
  Token tok = make_dummy_token("x", TokenKind::kIdentifier);
  auto type = make_node<TypeNode>(make_dummy_token("i32"), "i32");

  ParameterNode node(tok, "x", std::move(type));

  EXPECT_EQ(node.name, "x");
  EXPECT_TRUE(std::holds_alternative<std::unique_ptr<TypeNode>>(node.type));
}

TEST(ASTNodeTest, FunctionNodeTest) {
  Token tok = make_dummy_token("fn", TokenKind::kFn);
  ASTNode ret_type = make_node<TypeNode>(make_dummy_token("i32"), "i32");

  std::vector<ASTNode> params;
  params.push_back(make_node<ParameterNode>(
      make_dummy_token("x"), "x",
      make_node<TypeNode>(make_dummy_token("i32"), "i32")));
  ASTNode params_node = make_node<ParameterListNode>(
      make_dummy_token("(", lexer::TokenKind::kLParen), std::move(params));

  std::vector<ASTNode> block;
  block.push_back(make_node<ReturnNode>(
      make_dummy_token("return"),
      make_node<LiteralNode>(make_dummy_token("0"), LiteralNode::Type::kNumeric,
                             "0")));
  ASTNode body = make_node<BlockNode>(make_dummy_token("{"), std::move(block));

  std::string_view view = "add";
  FunctionNode node(tok, view, std::move(params_node), std::move(ret_type),
                    std::move(body));

  EXPECT_EQ(node.name, "add");
  // EXPECT_EQ(node.parameters, 1);
  EXPECT_TRUE(std::holds_alternative<std::unique_ptr<BlockNode>>(node.body));
}

}  // namespace ast
