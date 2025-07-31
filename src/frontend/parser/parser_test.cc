// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/parser/parser.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "core/base/logger.h"
#include "frontend/ast/base/base_node.h"
#include "frontend/ast/nodes/node_util.h"
#include "frontend/diagnostic/base/diagnostic_engine.h"
#include "frontend/lexer/token/token_kind.h"
#include "gtest/gtest.h"

namespace ast {

namespace {

std::unique_ptr<parser::Parser> make_parser(std::vector<lexer::Token>&& tokens,
                                            const core::FileManager* manager) {
  lexer::TokenStream stream(std::move(tokens), manager);
  return std::make_unique<parser::Parser>(std::move(stream));
}

}  // namespace

TEST(ParserTest, ParseLiteral) {
  std::vector<lexer::Token> tokens;
  core::FileManager manager;
  core::FileId file_id = manager.add_virtual_file("42;");

  tokens.emplace_back(lexer::TokenKind::kLiteralNumeric, file_id, 1, 1, 2);
  tokens.emplace_back(lexer::TokenKind::kSemicolon, file_id, 1, 3, 1);
  tokens.emplace_back(lexer::TokenKind::kEof, file_id, 1, 4, 0);
  auto parser = make_parser(std::move(tokens), &manager);
  auto result = parser->parse();
  EXPECT_TRUE(result.is_ok());

  const auto& program_node =
      std::get<std::unique_ptr<ProgramNode>>(result.unwrap());
  EXPECT_NE(program_node, nullptr);
  // DLOG(info, "{}\n", program_node->dump());
  EXPECT_FALSE(program_node->statements.empty());
}

TEST(ParserTest, ParseError) {
  std::vector<lexer::Token> tokens;
  core::FileManager manager;
  core::FileId file_id =
      manager.add_virtual_file("x := unknown_identifier unknown_identifier2;");

  tokens.emplace_back(lexer::TokenKind::kIdentifier, file_id, 1, 1, 1);
  tokens.emplace_back(lexer::TokenKind::kAssign, file_id, 1, 3, 2);
  tokens.emplace_back(lexer::TokenKind::kIdentifier, file_id, 1, 6, 18);
  tokens.emplace_back(lexer::TokenKind::kIdentifier, file_id, 1, 25, 19);
  tokens.emplace_back(lexer::TokenKind::kSemicolon, file_id, 1, 44, 1);
  tokens.emplace_back(lexer::TokenKind::kEof, file_id, 1, 45, 0);
  auto parser = make_parser(std::move(tokens), &manager);
  auto result = parser->parse();
  EXPECT_TRUE(result.is_err());
}

TEST(ParserTest, ParseSimpleVariableDeclaration) {
  std::vector<lexer::Token> tokens;
  core::FileManager manager;
  core::FileId file_id = manager.add_virtual_file("x := 42;");

  tokens.emplace_back(lexer::TokenKind::kIdentifier, file_id, 1, 1, 1);
  tokens.emplace_back(lexer::TokenKind::kAssign, file_id, 1, 3, 2);
  tokens.emplace_back(lexer::TokenKind::kLiteralNumeric, file_id, 1, 6, 2);
  tokens.emplace_back(lexer::TokenKind::kSemicolon, file_id, 1, 8, 1);
  tokens.emplace_back(lexer::TokenKind::kEof, file_id, 1, 9, 0);
  auto parser = make_parser(std::move(tokens), &manager);
  auto result = parser->parse();
  EXPECT_TRUE(result.is_ok());

  const auto& program_node = node_as<ProgramNode>(result.unwrap());
  EXPECT_NE(program_node, nullptr);
  // DLOG(info, "{}\n", program_node->dump());
  EXPECT_FALSE(program_node->statements.empty());

  // const auto& var_decl_node =
  //     std::get<std::unique_ptr<VariableDeclarationNode>>(
  //         program_node->statements[0]);
  // EXPECT_NE(var_decl_node, nullptr);
  // EXPECT_EQ(var_decl_node->name, "x");
  // EXPECT_EQ(var_decl_node->token.kind(), lexer::TokenKind::kIdentifier);
}

}  // namespace ast
