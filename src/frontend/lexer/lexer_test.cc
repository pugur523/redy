// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/lexer.h"

#include <string>
#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "gtest/gtest.h"

namespace lexer {

namespace {

void verify_ok(std::string&& source,
               base::TokenKind expected_kind,
               uint32_t n = 4) {
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(source));
  Lexer lexer(&manager.file(id));

  for (uint32_t i = 0; i < n; ++i) {
    auto result = lexer.next_token();
    ASSERT_TRUE(result.is_ok()) << result.unwrap_err().message;
    EXPECT_EQ(result.unwrap().kind(), expected_kind);
  }

  base::Token eof = lexer.next_token().unwrap();
  EXPECT_EQ(eof.kind(), base::TokenKind::kEof);
}

void verify_error(std::string&& source,
                  diagnostic::DiagnosticId expected_error_id) {
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(source));
  Lexer lexer(&manager.file(id));
  auto token_or_err = lexer.next_token();
  ASSERT_TRUE(token_or_err.is_err())
      << base::token_kind_to_string(token_or_err.unwrap().kind());
  EXPECT_EQ(token_or_err.unwrap_err().id, expected_error_id);
}

}  // namespace

TEST(LexerTest, LexSimpleCode) {
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file("x := 42;");
  Lexer lexer(&manager.file(id));

  base::Token t1 = lexer.next_token().unwrap();
  base::Token t2 = lexer.next_token().unwrap();
  base::Token t3 = lexer.next_token().unwrap();
  base::Token t4 = lexer.next_token().unwrap();
  base::Token t5 = lexer.next_token().unwrap();
  EXPECT_EQ(t1.kind(), base::TokenKind::kIdentifier);
  EXPECT_EQ(t2.kind(), base::TokenKind::kAssign);
  EXPECT_EQ(t3.kind(), base::TokenKind::kLiteralNumeric);
  EXPECT_EQ(t4.kind(), base::TokenKind::kSemicolon);
  EXPECT_EQ(t5.kind(), base::TokenKind::kEof);
}

TEST(LexerTest, HelloWorldFunction) {
  std::string source = R"(
        fn main() -> i32 {
            world_str := "world";
            x: i32 = 42;
            println#("hello {}", world_str);
            println#("answer to the ultimate question of life, the universe, and everything is {}.", x);
            ret 0;
        }
    )";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(source));
  const core::File& file = manager.file(id);
  Lexer lexer(&manager.file(id));

  while (true) {
    base::Token token = lexer.next_token().unwrap();

    EXPECT_FALSE(std::string(token_kind_to_string(token.kind())).empty());

    EXPECT_GT(token.start().line(), 0);
    EXPECT_GT(token.start().column(), 0);

    EXPECT_NE(token.kind(), base::TokenKind::kUnknown);

    if (token.kind() == base::TokenKind::kEof) {
      EXPECT_TRUE(token.lexeme(file).empty());
      break;
    } else {
      EXPECT_FALSE(token.lexeme(file).empty());
    }
  }
}

TEST(LexerTest, HelloWorldWithUnicodeCharacters) {
  std::string source = R"(
      fn メイン() -> i32 {
        ワールド := "ワールド";
        println#("ハロー{}", ワールド);
        ret 0;
      } 
    )";
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(source));
  const core::File& file = manager.file(id);
  Lexer lexer(&manager.file(id));

  while (true) {
    base::Token token = lexer.next_token().unwrap();

    EXPECT_FALSE(std::string(token_kind_to_string(token.kind())).empty());

    EXPECT_GT(token.start().line(), 0);
    EXPECT_GT(token.start().column(), 0);

    EXPECT_NE(token.kind(), base::TokenKind::kUnknown);

    if (token.kind() == base::TokenKind::kEof) {
      EXPECT_TRUE(token.lexeme(file).empty());
      break;
    } else {
      EXPECT_FALSE(token.lexeme(file).empty());
    }
  }
}

TEST(LexerTest, UnicodeCharacter) {
  verify_ok("α 中　", base::TokenKind::kIdentifier, 2);
}

TEST(LexerErrorTest, UnterminatedStringLiteral) {
  verify_error(R"("hello)",
               diagnostic::DiagnosticId::kUnterminatedStringLiteral);
}

TEST(LexerErrorTest, UnterminatedCharacterLiteral) {
  verify_error(R"('x)",
               diagnostic::DiagnosticId::kUnterminatedCharacterLiteral);
}

TEST(LexerErrorTest, UnterminatedBlockComment) {
  verify_error("/* unterminated",
               diagnostic::DiagnosticId::kUnterminatedBlockComment);
}

TEST(LexerErrorTest, UnexpectedEof) {
  std::string source;
  source.push_back('\0');
  verify_error(std::move(source),
               diagnostic::DiagnosticId::kUnexpectedEndOfFile);
}

TEST(LexerErrorTest, UnrecognizedCharacter) {
  verify_error("¿", diagnostic::DiagnosticId::kUnrecognizedCharacter);
}

TEST(LexerErrorTest, UnrecognizedUnicodeCharacter) {
  verify_error("☄", diagnostic::DiagnosticId::kUnrecognizedCharacter);
  verify_error("🚀", diagnostic::DiagnosticId::kUnrecognizedCharacter);
  verify_error("☺", diagnostic::DiagnosticId::kUnrecognizedCharacter);
}

TEST(LexerErrorTest, UnrecognizedControlCharacter) {
  std::string source;
  source.push_back('\x01');
  verify_error(std::move(source),
               diagnostic::DiagnosticId::kUnrecognizedCharacter);
}

TEST(LexerErrorTest, InvalidEscapeSequence) {
  verify_error(R"("bad \q escape")",
               diagnostic::DiagnosticId::kInvalidEscapeSequence);
}

// numeric literal error tests
TEST(LexerErrorTest, EmptyHexLiteral) {
  verify_error("0x", diagnostic::DiagnosticId::kInvalidNumericLiteral);
}

TEST(LexerErrorTest, EmptyBinaryLiteral) {
  verify_error("0b", diagnostic::DiagnosticId::kInvalidNumericLiteral);
}

TEST(LexerErrorTest, EmptyOctalLiteral) {
  verify_error("0o", diagnostic::DiagnosticId::kInvalidNumericLiteral);
}

TEST(LexerErrorTest, InvalidNumericSuffix) {
  verify_error("42xyz", diagnostic::DiagnosticId::kInvalidNumericLiteral);
}

// edge cases
TEST(LexerTest, ZeroWithDifferentBases) {
  verify_ok("0 0x0 0b0 0o0", base::TokenKind::kLiteralNumeric);
}

TEST(LexerTest, kFloatingPointEdgeCases) {
  verify_ok("0.0 1e0 1E0 1e+0 1e-0", base::TokenKind::kLiteralNumeric, 5);
}

}  // namespace lexer
