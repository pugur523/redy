// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "benchmark/benchmark.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

namespace {

void token_construction(benchmark::State& state) {
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file("42");
  for (auto _ : state) {
    Token tok(TokenKind::kLiteralNumber, &manager, id, 1, 1, 2);
    benchmark::DoNotOptimize(tok);
  }
}

BENCHMARK(token_construction);

}  // namespace

}  // namespace lexer
