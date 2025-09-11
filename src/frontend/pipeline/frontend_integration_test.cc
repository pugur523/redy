// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/base/logger.h"
#include "frontend/base/string/string_interner.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/engine/diagnostic_engine.h"
#include "frontend/processor/lexer/lexer.h"
#include "frontend/processor/parser/parser.h"
#include "gtest/gtest.h"
#include "i18n/base/translator.h"
#include "testing/test_util.h"
#include "unicode/utf8/file_manager.h"

namespace {

void print_errors(diagnostic::DiagnosticEngine&& engine) {
  EXPECT_FALSE(engine.entries().empty());
  if (!engine.entries().empty()) {
    const std::string formatted_diagnostics = engine.format_batch_and_clear();
    DLOG(info, "\n{}", formatted_diagnostics);
  }
}

void verify_compile_pipeline(std::u8string&& src) {
  unicode::Utf8FileManager manager;
  unicode::Utf8FileId id = manager.register_virtual_file(std::move(src));
  base::StringInterner interner;
  i18n::Translator translator;
  diagnostic::DiagnosticOptions options;
  diagnostic::DiagnosticEngine engine(&manager, &translator, options);

  lexer::Lexer lexer;

  lexer::Lexer::InitResult init_result = lexer.init(&manager, id);
  if (init_result.is_err()) {
    engine.push(std::move(init_result).unwrap_err());
    print_errors(std::move(engine));
    return;
  }

  lexer::Lexer::Results<base::Token> tokenize_result = lexer.tokenize();
  if (tokenize_result.is_err()) {
    for (auto&& e : std::move(tokenize_result).unwrap_err()) {
      engine.push(std::move(e).convert_to_entry());
    }
    print_errors(std::move(engine));
    return;
  }
  std::vector<base::Token> tokens = std::move(tokenize_result).unwrap();
  EXPECT_FALSE(tokens.empty());
  base::TokenStream stream(std::move(tokens), &manager, id);
  // DLOG(info, "{}", stream.dump());

  parser::Parser parser;
  parser.init(&stream, &interner, translator);
  auto result = parser.parse_all(false);
  manager.unload(id);
  EXPECT_TRUE(result.is_ok());
  if (result.is_err()) {
    engine.push(std::move(result).unwrap_err());
    print_errors(std::move(engine));
    return;
  }

  std::unique_ptr<ast::Context> context = std::move(result).unwrap();

  return;
}

}  // namespace

TEST(FrontendTest, SimpleCodePipeline) {
  verify_compile_pipeline(u8"x := 42; y: i32 = 57;");
}

// TEST(FrontendTest, HelloWorldFunctionPipeline) {
//   unicode::Utf8FileManager manager;
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
//   unicode::Utf8FileId id = manager.add_virtual_file(std::move(source));
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
