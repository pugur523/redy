// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>

#include "benchmark/benchmark.h"
#include "frontend/base/keyword/keyword.h"

namespace base {

static void keyword_lookup_keyword(benchmark::State& state) {
  const std::u8string id = u8"if";
  for (auto _ : state) {
    benchmark::DoNotOptimize(lookup_id_or_keyword(id));
  }

  state.SetBytesProcessed(id.size() * sizeof(char) * state.iterations());
}
BENCHMARK(keyword_lookup_keyword);

}  // namespace base
