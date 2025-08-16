// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/lexer/lexer.h"

#include <string>
#include <utility>
#include <vector>

#include "frontend/base/token/token_kind.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "gtest/gtest.h"
#include "unicode/utf8/file_manager.h"

namespace lexer {

namespace {

struct TestLexer {
  unicode::Utf8FileManager manager;
  Lexer lexer;

  explicit TestLexer(std::u8string source,
                     Lexer::Mode mode = Lexer::Mode::kCodeAnalysis) {
    id_ = manager.register_virtual_file(std::move(source));
    const auto _ = lexer.init(manager.file(id_), mode);
  }

  base::Token next() {
    auto result = lexer.tokenize_next();
    EXPECT_TRUE(result.is_ok());
    if (result.is_err()) {
      // const unicode::Utf8Cursor& cursor = lexer.cursor();
      // int diag_id = static_cast<int>(result.unwrap_err().diag_id);

      // std::cerr << "diag id: " << diag_id << '\n'
      //           << "pos: " << cursor.position() << '\n'
      //           << "line: " << cursor.line() << '\n'
      //           << "column: " << cursor.column() << '\n';
    }
    return std::move(result).unwrap();
  }

  void expect_error_next(diagnostic::DiagnosticId diag_id) {
    auto result = lexer.tokenize_next();
    EXPECT_TRUE(result.is_err());
    if (result.is_ok()) {
      // auto token = std::move(result).unwrap();
      // const unicode::Utf8Cursor& cursor = lexer.cursor();
      // std::cerr << "lexeme: " << token.lexeme(manager.file(id_)) << '\n'
      //           << "pos: " << cursor.position() << '\n'
      //           << "line: " << cursor.line() << '\n'
      //           << "column: " << cursor.column() << '\n';
    } else {
      EXPECT_EQ(std::move(result).unwrap_err().diag_id, diag_id);
    }
  }

 private:
  unicode::Utf8FileId id_;
};

void expect_tokens(std::u8string source,
                   std::vector<base::TokenKind>&& expected_kinds,
                   Lexer::Mode mode = Lexer::Mode::kCodeAnalysis) {
  TestLexer test_lexer(std::move(source), mode);

  for (base::TokenKind expected : expected_kinds) {
    EXPECT_EQ(test_lexer.next().kind(), expected);
  }

  EXPECT_EQ(test_lexer.next().kind(), base::TokenKind::kEof);
}

void expect_error(std::u8string source,
                  diagnostic::DiagnosticId expected_error) {
  TestLexer test_lexer(std::move(source));
  test_lexer.expect_error_next(expected_error);
}

void expect_repeated_token(std::u8string source,
                           base::TokenKind expected_kind,
                           uint32_t count,
                           Lexer::Mode mode = Lexer::Mode::kCodeAnalysis) {
  TestLexer test_lexer(std::move(source), mode);

  for (uint32_t i = 0; i < count; ++i) {
    EXPECT_EQ(test_lexer.next().kind(), expected_kind);
  }

  EXPECT_EQ(test_lexer.next().kind(), base::TokenKind::kEof);
}

void validate_token_properties(std::u8string source,
                               Lexer::Mode mode = Lexer::Mode::kCodeAnalysis) {
  TestLexer test_lexer(std::move(source), mode);
  const unicode::Utf8FileId id =
      test_lexer.manager.register_virtual_file(std::u8string(source));
  const unicode::Utf8File& file = test_lexer.manager.file(id);

  while (true) {
    const base::Token token = test_lexer.next();

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

}  // namespace

TEST(LexerTest, LexSimpleCode) {
  expect_tokens(
      u8"x := 42;",
      {base::TokenKind::kIdentifier, base::TokenKind::kColonEqual,
       base::TokenKind::kLiteralDecimal, base::TokenKind::kSemicolon});
}

TEST(LexerTest, HelloWorldFunction) {
  std::u8string source =
      u8R"(
    fn main() -> i32 {
        world_str := "world";
        x: i32 = 42;
        println#("hello {}", world_str);
        println#("answer to the ultimate question of life, the universe, and
        everything is {}.", x); ret 0;
    }
  )";
  validate_token_properties(std::move(source));
}

TEST(LexerTest, HelloWorldWithUnicodeCharacters) {
  std::u8string source =
      u8R"(
    fn メイン() -> i32 {
      ワールド := "ワールド";
      println#("ハロー{}", ワールド);
      ret 0;
    }
  )";
  validate_token_properties(std::move(source));
}

TEST(LexerTest, UnicodeCharacter) {
  expect_repeated_token(u8"α 中　", base::TokenKind::kIdentifier, 2);
}

// lexer mode tests
TEST(LexerModeTest, InlineCommentIgnore) {
  // ignore inline comment in code analysis mode
  expect_tokens(u8"// comment\nx",
                {base::TokenKind::kNewline, base::TokenKind::kIdentifier},
                Lexer::Mode::kCodeAnalysis);
}

TEST(LexerModeTest, InlineCommentStore) {
  // store inline comment in format mode
  expect_tokens(u8"// comment\nx",
                {base::TokenKind::kInlineComment, base::TokenKind::kNewline,
                 base::TokenKind::kIdentifier},
                Lexer::Mode::kFormat);
}

TEST(LexerModeTest, BlockCommentIgnore) {
  // ignore block comment in code analysis mode
  expect_tokens(u8"/* comment */ x", {base::TokenKind::kIdentifier},
                Lexer::Mode::kCodeAnalysis);
}

TEST(LexerModeTest, BlockCommentStore) {
  // store block comment in format mode
  expect_tokens(u8"/* comment */ x",
                {base::TokenKind::kBlockComment, base::TokenKind::kWhitespace,
                 base::TokenKind::kIdentifier},
                Lexer::Mode::kFormat);
}

TEST(LexerModeTest, DocumentationCommentIgnore) {
  // ignore doc comment in code analysis mode
  expect_tokens(u8"//@ doc\nx",
                {base::TokenKind::kNewline, base::TokenKind::kIdentifier},
                Lexer::Mode::kCodeAnalysis);
}

TEST(LexerModeTest, DocumentationCommentStore) {
  // store doc comment in document generation mode
  expect_tokens(u8"//@ doc\nx",
                {base::TokenKind::kDocumentationComment,
                 base::TokenKind::kNewline, base::TokenKind::kIdentifier},
                Lexer::Mode::kDocumentGen);
}

TEST(LexerModeTest, WhitespaceAndNewlineInFormat) {
  // unicode whitespaces: U+2003 EM SPACE, U+2028 LINE SEPARATOR
  std::u8string src = u8"x\u2003y\u2028z";

  expect_tokens(std::move(src),
                {
                    base::TokenKind::kIdentifier,  // x
                    base::TokenKind::kWhitespace,  // EM SPACE
                    base::TokenKind::kIdentifier,  // y
                    base::TokenKind::kNewline,     // LINE SEPARATOR
                    base::TokenKind::kIdentifier   // z
                },
                Lexer::Mode::kFormat);
}

// error tests
TEST(LexerErrorTest, UnterminatedLiterals) {
  expect_error(u8R"("hello)",
               diagnostic::DiagnosticId::kUnterminatedStringLiteral);
  expect_error(u8R"('x)",
               diagnostic::DiagnosticId::kUnterminatedCharacterLiteral);
  expect_error(u8"/* unterminated",
               diagnostic::DiagnosticId::kUnterminatedBlockComment);
}

TEST(LexerErrorTest, UnexpectedCharacters) {
  expect_error(u8"¿", diagnostic::DiagnosticId::kUnrecognizedCharacter);
  expect_error(u8"☄", diagnostic::DiagnosticId::kUnrecognizedCharacter);
  expect_error(u8"🚀", diagnostic::DiagnosticId::kUnrecognizedCharacter);

  // control character
  std::u8string control_char;
  control_char.push_back('\x01');
  expect_error(std::move(control_char),
               diagnostic::DiagnosticId::kUnrecognizedCharacter);

  // null character
  std::u8string null_char;
  null_char.push_back('\0');
  expect_error(std::move(null_char),
               diagnostic::DiagnosticId::kUnexpectedEndOfFile);
}

TEST(LexerErrorTest, InvalidEscapeSequence) {
  expect_error(u8R"("bad \q escape")",
               diagnostic::DiagnosticId::kInvalidEscapeSequence);
}

TEST(LexerErrorTest, InvalidNumericLiterals) {
  expect_error(u8"0x", diagnostic::DiagnosticId::kInvalidNumericLiteral);
  expect_error(u8"0b", diagnostic::DiagnosticId::kInvalidNumericLiteral);
  expect_error(u8"0o", diagnostic::DiagnosticId::kInvalidNumericLiteral);
  expect_error(u8"42xyz", diagnostic::DiagnosticId::kInvalidNumericLiteral);
}

// edge cases
TEST(LexerTest, NumericLiteralEdgeCases) {
  expect_tokens(
      u8"0 0x0 0b0 0o0",
      {base::TokenKind::kLiteralDecimal, base::TokenKind::kLiteralHexadecimal,
       base::TokenKind::kLiteralBinary, base::TokenKind::kLiteralOctal});
  expect_repeated_token(u8"0.0 1e0 1E0 1e+0 1e-0",
                        base::TokenKind::kLiteralDecimal, 5);
}

}  // namespace lexer
