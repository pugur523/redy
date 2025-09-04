// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/parser/parser.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "frontend/base/token/token_kind.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "gtest/gtest.h"
#include "i18n/base/translator.h"
#include "unicode/utf8/file_manager.h"

namespace parser {

namespace {

const i18n::Translator translator;
unicode::Utf8FileManager file_manager;
unicode::Utf8FileId dummy_id = 0;

base::TokenStream tkstr(std::vector<base::TokenKind>&& kinds) {
  std::vector<base::Token> tokens;
  tokens.reserve(kinds.size());

  for (auto kind : kinds) {
    base::Token token(kind, 0, 0, 0);
    tokens.emplace_back(std::move(token));
  }

  return base::TokenStream(std::move(tokens), &file_manager, dummy_id);
}

struct TestParser {
  Parser parser;

  explicit TestParser(base::TokenStream&& stream) : stream_(std::move(stream)) {
    parser.init(&stream_, translator);
  }

  explicit TestParser(std::vector<base::TokenKind>&& kinds)
      : stream_(tkstr(std::move(kinds))) {
    parser.init(&stream_, translator);
  }

  void expect_ok(bool strict = false) {
    auto result = parser.parse_all(strict);

    EXPECT_TRUE(result.is_ok());
    if (result.is_err()) {
      // int diag_id = static_cast<int>(result.unwrap_err().diag_id);

      // std::cerr << "diag id: " << diag_id << '\n'
      //           << "pos: " << stream.position() << '\n'
      //           << "line: " << stream.line() << '\n'
      //           << "column: " << stream.column() << '\n';
      return;
    }
  }

  void expect_errors(std::vector<diagnostic::DiagnosticId> diag_ids) {
    auto result = parser.parse_all(false);
    EXPECT_TRUE(result.is_err());
    if (result.is_ok()) {
      // auto token = std::move(result).unwrap();
      // const unicode::Utf8Stream& stream = parser.stream();
      // std::cerr << "lexeme: " << token.lexeme(manager.file(id_)) << '\n'
      //           << "pos: " << stream.position() << '\n'
      //           << "line: " << stream.line() << '\n'
      //           << "column: " << stream.column() << '\n';
      return;
    }
    const auto errors = std::move(result).unwrap_err();
    EXPECT_EQ(errors.size(), diag_ids.size());

    const std::size_t min_size = std::min(errors.size(), diag_ids.size());
    for (std::size_t i = 0; i < min_size; ++i) {
      EXPECT_EQ(errors[i].header().diag_id(), diag_ids[i]);
    }
  }

 private:
  base::TokenStream stream_;
};

void expect_error(base::TokenStream&& stream,
                  diagnostic::DiagnosticId expected_error) {
  TestParser test_parser(std::move(stream));
  test_parser.expect_errors({expected_error});
}

void expect_errors(base::TokenStream&& stream,
                   std::vector<diagnostic::DiagnosticId>&& expected_errors) {
  TestParser test_parser(std::move(stream));
  test_parser.expect_errors(std::move(expected_errors));
}

}  // namespace

TEST(ParserTest, ParseSimple) {
  TestParser parser({
      base::TokenKind::kFunction,
      base::TokenKind::kIdentifier,
      base::TokenKind::kLeftParen,
      base::TokenKind::kRightParen,
      base::TokenKind::kEof,
  });
  parser.expect_ok();
}

}  // namespace parser
