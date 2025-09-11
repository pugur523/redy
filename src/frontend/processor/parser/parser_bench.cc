// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "benchmark/benchmark.h"
#include "frontend/base/string/string_interner.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

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

void parser_init(benchmark::State& state) {
  auto stream = tkstr({base::TokenKind::kEof});
  for (auto _ : state) {
    Parser parser;
    parser.init(&stream, &interner, translator);
    benchmark::DoNotOptimize(&parser);
  }
  state.SetBytesProcessed(sizeof(Parser) * state.iterations());
}
BENCHMARK(parser_init);

void parser_parse_empty_stream(benchmark::State& state) {
  auto stream = tkstr({base::TokenKind::kEof});
  Parser parser;
  parser.init(&stream, &interner, translator);
  for (auto _ : state) {
    auto result = parser.parse_all();
    benchmark::DoNotOptimize(std::move(result).unwrap().get());
    parser.reset();
  }
  state.SetBytesProcessed(sizeof(Parser) * state.iterations());
}
BENCHMARK(parser_parse_empty_stream);

void parser_parse_simple_func(benchmark::State& state) {
  auto stream = tkstr({
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
  // fn some_func(x: i32, y: str) -> bool {}

  Parser parser;
  parser.init(&stream, &interner, translator);
  for (auto _ : state) {
    auto result = parser.parse_all();
    benchmark::DoNotOptimize(std::move(result).unwrap().get());
    parser.reset();
  }
  state.SetBytesProcessed(sizeof(base::Token) * stream.size() *
                          state.iterations());
}
BENCHMARK(parser_parse_simple_func);

}  // namespace

}  // namespace parser
