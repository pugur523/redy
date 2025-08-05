// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>
#include <vector>

#include "core/base/file_util.h"
#include "core/base/logger.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/lexer/lexer.h"
#include "gtest/gtest.h"
#include "testing/test_util.h"

TEST(FrontendTest, SimpleCodePipeline) {
  core::FileManager manager;

  // core::FileId id = manager.add_file(file_path);
  core::FileId id = manager.add_virtual_file("x := 42; y: i32 = 57;");
  const core::File& file = manager.file(id);

  lexer::Lexer lexer(&file);

  std::vector<base::Token> tokens = lexer.lex_all().unwrap();
  EXPECT_FALSE(tokens.empty());
  base::TokenStream stream(std::move(tokens), &file);
  // DLOG(info, "{}", stream.dump());

  // parser::Parser parser(std::move(stream));

  // auto result = parser.parse();
  // EXPECT_TRUE(result.is_ok());
  // const auto& program_node =
  //     std::get<std::unique_ptr<ast::ProgramNode>>(result.unwrap());
  // EXPECT_FALSE(program_node->statements.empty());
  // // DLOG(info, "{}\n", program_node->dump());
  // core::write_file(
  //     core::join_path(test_dir(),
  //     "frontend_simple_code_pipeline.ast").c_str(), program_node->dump());
}

// TEST(FrontendTest, HelloWorldFunctionPipeline) {
//   core::FileManager manager;
//   std::string source = R"(
//         fn main() -> i32 {
//             world_str := "world"
//             x: i32 = 42
//             println#("hello {}", world_str)
//             println#("answer to the ultimate question of life, the universe,
//                     and everything is {}.", x)
//             ret 0
//         }
//     )";
//   core::FileId id = manager.add_virtual_file(std::move(source));
//   lexer::Lexer lexer(&manager, id);
//
//   auto tokens = lexer.lex_all().unwrap();
//   EXPECT_FALSE(tokens.empty());
//   base::TokenStream stream(std::move(tokens), &manager);
//
//   parser::Parser parser(std::move(stream));
//
//   auto result = parser.parse();
//   EXPECT_TRUE(result.is_ok());
//   const auto& program_node =
//       std::get<std::unique_ptr<ast::ProgramNode>>(result.unwrap());
//   EXPECT_FALSE(program_node->statements.empty());
//   DLOG(info, "{}", program_node->dump());
//   // core::write_file(
//   //     core::join_path(test_dir(),
//   //     "frontend_hello_world_function_pipeline.ast")
//   //         .c_str(),
//   //     program_node->dump());
// }
