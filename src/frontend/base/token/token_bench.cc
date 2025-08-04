// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "benchmark/benchmark.h"
#include "frontend/base/token/token.h"

namespace base {

namespace {

void token_construction(benchmark::State& state) {
  for (auto _ : state) {
    Token tok(TokenKind::kLiteralNumeric, 1, 1, 2);
    benchmark::DoNotOptimize(tok.kind());
    benchmark::DoNotOptimize(tok.start().line());
    benchmark::DoNotOptimize(tok.start().column());
    benchmark::DoNotOptimize(tok.length());
  }

  state.SetBytesProcessed(sizeof(Token) * state.iterations());
}

BENCHMARK(token_construction);

}  // namespace

}  // namespace base
