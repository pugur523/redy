// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/parser/parser.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "frontend/base/string/string_interner.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/engine/diagnostic_engine.h"
#include "gtest/gtest.h"
#include "i18n/base/translator.h"
#include "unicode/utf8/file_manager.h"

namespace parser {

namespace {

base::StringInterner interner;
const i18n::Translator translator;
unicode::Utf8FileManager file_manager;

base::TokenStream tkstr(std::vector<base::TokenKind>&& kinds) {
  std::vector<base::Token> tokens;
  tokens.reserve(kinds.size());

  for (auto kind : kinds) {
    base::Token token(kind, 1, 1, 0);
    tokens.emplace_back(std::move(token));
  }
  const unicode::Utf8FileId id = file_manager.register_virtual_file(u8"");
  return base::TokenStream(std::move(tokens), &file_manager, id);
}

struct TestParser {
  Parser parser;

  explicit TestParser(base::TokenStream&& stream) : stream_(std::move(stream)) {
    parser.init(&stream_, &interner, translator);
  }

  explicit TestParser(std::vector<base::TokenKind>&& kinds)
      : stream_(tkstr(std::move(kinds))) {
    parser.init(&stream_, &interner, translator);
  }

  ~TestParser() = default;

  void expect_ok(bool strict = false) {
    auto result = parser.parse_all(strict);

    EXPECT_TRUE(result.is_ok());
    if (result.is_err()) {
      describe_errors(std::move(result).unwrap_err());
    }
  }

  void expect_errors(std::vector<diagnostic::DiagnosticId> diag_ids) {
    auto result = parser.parse_all(false);
    EXPECT_TRUE(result.is_err());
    if (result.is_ok()) {
      return;
    }
    auto errors = std::move(result).unwrap_err();
    EXPECT_EQ(errors.size(), diag_ids.size());

    const std::size_t min_size = std::min(errors.size(), diag_ids.size());
    for (std::size_t i = 0; i < min_size; ++i) {
      EXPECT_EQ(errors[i].header().diag_id(), diag_ids[i]);
    }

    if (errors.size() != diag_ids.size()) {
      describe_errors(std::move(errors));
    }
  }

 private:
  static void describe_errors(
      std::vector<diagnostic::DiagnosticEntry>&& errors) {
    for (const auto& entry : errors) {
      std::cerr << "diag id: "
                << translator.translate(diagnostic::diagnostic_id_to_tr_key(
                       entry.header().diag_id()))
                << '\n';
      for (const auto& label : entry.labels()) {
        std::cerr << "label: " << translator.translate(label.message_tr_key())
                  << '\n';
        for (uint8_t i = 0; i < label.args_count(); ++i) {
          std::cerr << "format args " << i << ": " << label.format_args()[i]
                    << '\n';
        }
        for (const auto& ann : label.annotations()) {
          std::cerr << "ann: " << translator.translate(ann.message_tr_key())
                    << '\n';
        }
      }
    }
  }

  base::TokenStream stream_;
};

}  // namespace

TEST(ParserTest, ParseEmptyStruct) {
  TestParser parser({
      base::TokenKind::kStruct,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // struct String{}
  parser.expect_ok();
}

TEST(ParserTest, ParseEmptyFunction) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kArrow,
      base::TokenKind::kI32,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // fn some_name() -> i32 {}
  parser.expect_ok();
}

TEST(ParserTest, ParseFunctionWithParameters) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kI32,
      base::TokenKind::kComma,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kStr,
      base::TokenKind::kRightParen,
      base::TokenKind::kArrow,
      base::TokenKind::kBool,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // fn process(x: i32, msg: str) -> bool {}
  parser.expect_ok();
}

TEST(ParserTest, ParsePublicFunction) {
  TestParser parser({
      base::TokenKind::kPublic,
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kArrow,
      base::TokenKind::kVoid,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // pub fn main() -> void {}
  parser.expect_ok();
}

TEST(ParserTest, ParseAsyncFunction) {
  TestParser parser({
      base::TokenKind::kAsync,
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kArrow,
      base::TokenKind::kI32,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // async fn fetch_data() -> i32 {}
  parser.expect_ok();
}

TEST(ParserTest, ParseStructWithFields) {
  TestParser parser({
      base::TokenKind::kStruct,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kI32,
      base::TokenKind::kComma,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kStr,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // struct Person { age: i32, name: str }
  parser.expect_ok();
}

TEST(ParserTest, ParsePublicStruct) {
  TestParser parser({
      base::TokenKind::kPublic,
      base::TokenKind::kStruct,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // pub struct Config {}
  parser.expect_ok();
}

TEST(ParserTest, ParseEmptyEnum) {
  TestParser parser({
      base::TokenKind::kEnumeration,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // enum Status {}
  parser.expect_ok();
}

TEST(ParserTest, ParseEnumWithVariants) {
  TestParser parser({
      base::TokenKind::kEnumeration,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kIdentifier,
      base::TokenKind::kComma,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kI32,
      base::TokenKind::kRightParen,
      base::TokenKind::kComma,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // enum Result { ok, err(i32) }
  parser.expect_ok();
}

TEST(ParserTest, ParseEmptyTrait) {
  TestParser parser({
      base::TokenKind::kTrait,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // trait Display {}
  parser.expect_ok();
}

TEST(ParserTest, ParseUnionWithFields) {
  TestParser parser({
      base::TokenKind::kUnion,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kI32,
      base::TokenKind::kComma,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kF32,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // union Value { i: i32, f: f32 }
  parser.expect_ok();
}

TEST(ParserTest, ParseEmptyModule) {
  TestParser parser({
      base::TokenKind::kModule,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // mod utils {}
  parser.expect_ok();
}

TEST(ParserTest, ParseConstantDeclaration) {
  TestParser parser({
      base::TokenKind::kConstant,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kI32,
      base::TokenKind::kEqual,
      base::TokenKind::kDecimal,
      base::TokenKind::kEof,
  });
  // const kMaxSize: i32 = 100
  parser.expect_ok();
}

TEST(ParserErrorTest, MissingFunctionBody) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kArrow,
      base::TokenKind::kI32,
      base::TokenKind::kEof,
  });
  // fn func() -> i32 (missing body)
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, MissingStructBraces) {
  TestParser parser({
      base::TokenKind::kStruct,
      base::TokenKind::kIdentifier,
      base::TokenKind::kEof,
  });
  // struct Name (missing braces)
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, UnexpectedTokenInFunction) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kStar,  // expected (
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kEof,
  });
  // fn func * ()
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, MalformedFunctionParameters) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kIdentifier,
      base::TokenKind::kComma,  // expected type but found comma
      base::TokenKind::kRightParen,
      base::TokenKind::kEof,
  });
  // fn func(param, )
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, ConflictingStorageSpecifier) {
  TestParser parser({
      base::TokenKind::kStatic,
      base::TokenKind::kExtern,  // conflict with static
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // static extern fn some_name() {}
  parser.expect_errors({
      diagnostic::DiagnosticId::kConflictingStorageSpecifiers,
  });
}

TEST(ParserErrorTest, ConflictingStorageSpecifiers2) {
  TestParser parser({
      base::TokenKind::kConstant,
      base::TokenKind::kMutable,
      base::TokenKind::kIdentifier,
      base::TokenKind::kEof,
  });
  // const mut variable (conflicting)
  parser.expect_errors({
      diagnostic::DiagnosticId::kConflictingStorageSpecifiers,
  });
}

TEST(ParserErrorTest, UnexpectedKeywordInStructBody) {
  TestParser parser({
      base::TokenKind::kStruct,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kIf,  // unexpected keyword
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // struct Name { if }
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, MissingReturnType) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kArrow,
      // missing return type
      base::TokenKind::kLeftBrace,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // fn func() -> {}
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, MissingCommaInParameters) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      base::TokenKind::kI32,
      base::TokenKind::kIdentifier,  // missing comma
      base::TokenKind::kColon,
      base::TokenKind::kStr,
      base::TokenKind::kRightParen,
      base::TokenKind::kEof,
  });
  // fn func(a: i32 b: str)
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, MissingFieldType) {
  TestParser parser({
      base::TokenKind::kStruct,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftBrace,
      base::TokenKind::kIdentifier,
      base::TokenKind::kColon,
      // missing field type
      base::TokenKind::kComma,
      base::TokenKind::kRightBrace,
      base::TokenKind::kEof,
  });
  // struct Person { name: , }
  parser.expect_errors({
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

TEST(ParserErrorTest, MultipleErrors) {
  TestParser parser({
      base::TokenKind::kStatic,
      base::TokenKind::kExtern,
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kStar,  // expected (
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kEof,
  });
  // static extern fn func * ()
  parser.expect_errors({
      diagnostic::DiagnosticId::kConflictingStorageSpecifiers,
      diagnostic::DiagnosticId::kExpectedButFound,
  });
}

}  // namespace parser
