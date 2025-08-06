// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <algorithm>
#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "frontend/lexer/lexer.h"

namespace lexer {

namespace {

void lexer_loop(benchmark::State& state) {
  std::string code = "x := 42 while x < 100 { x = x + 1 }";
  std::size_t code_size = code.size();

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(code));
  Lexer lexer(manager.file(id));

  for (auto _ : state) {
    while (true) {
      base::Token token = lexer.next_token().unwrap();
      if (token.kind() == base::TokenKind::kEof) {
        break;
      }
    }
  }
  state.SetBytesProcessed(code_size * sizeof(char) * state.iterations());
}

}  // namespace

BENCHMARK(lexer_loop);

}  // namespace lexer
