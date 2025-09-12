// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <algorithm>
#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "frontend/processor/lexer/lexer.h"

namespace lexer {

namespace {

void lexer_init(benchmark::State& state) {
  std::u8string code =
      u8"x := 42 while x < 100 { x = x + 1 } for i: 0..<100 { ++i } ret 0";
  std::size_t code_size = code.size();

  unicode::Utf8FileManager manager;
  unicode::Utf8FileId id = manager.register_virtual_file(std::move(code));

  for (auto _ : state) {
    Lexer lexer;
    auto init_result = lexer.init(&manager, id);
    benchmark::DoNotOptimize(init_result.is_ok());
  }
  state.SetBytesProcessed(code_size * state.iterations());
}
BENCHMARK(lexer_init);

void lexer_loop(benchmark::State& state) {
  std::u8string code =
      u8"x := 42 while x < 100 { x = x + 1 } for i: 0..<100 { ++i } ret 0";
  std::size_t code_size = code.size();

  unicode::Utf8FileManager manager;
  unicode::Utf8FileId id = manager.register_virtual_file(std::move(code));

  Lexer lexer;
  auto init_result = lexer.init(&manager, id);
  for (auto _ : state) {
    while (true) {
      const base::Token token = lexer.tokenize_next().unwrap();
      benchmark::DoNotOptimize(&token);
      if (token.kind() == base::TokenKind::kEof) {
        break;
      }
    }
    lexer.reset();
  }
  state.SetBytesProcessed(code_size * state.iterations());
}
BENCHMARK(lexer_loop);

void lexer_tokenize_bulk(benchmark::State& state) {
  std::u8string code =
      u8"x := 42 while x < 100 { x = x + 1 } for i: 0..<100 { ++i } ret 0";
  std::size_t code_size = code.size();

  unicode::Utf8FileManager manager;
  unicode::Utf8FileId id = manager.register_virtual_file(std::move(code));

  Lexer lexer;
  auto init_result = lexer.init(&manager, id);
  for (auto _ : state) {
    auto result = lexer.tokenize();
    benchmark::DoNotOptimize(std::move(result).unwrap().size());
    lexer.reset();
  }
  state.SetBytesProcessed(code_size * state.iterations());
}
BENCHMARK(lexer_tokenize_bulk);

}  // namespace

}  // namespace lexer
