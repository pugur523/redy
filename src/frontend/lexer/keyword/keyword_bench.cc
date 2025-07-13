// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "benchmark/benchmark.h"
#include "frontend/lexer/keyword/keyword.h"

namespace lexer {

static void keyword_lookup_keyword(benchmark::State& state) {
  const std::string id = "if";
  for (auto _ : state) {
    TokenKind k = lookup_identifier_or_keyword(id);
    benchmark::DoNotOptimize(k);
  }
}
BENCHMARK(keyword_lookup_keyword);

}  // namespace lexer
