// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "frontend/base/data/char_stream.h"

namespace base {

void char_stream_advance(benchmark::State& state) {
  std::string input(state.range(0), 'a');
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager, id);
  for (auto _ : state) {
    while (!cs.eof()) {
      cs.advance();
    }
  }
}
BENCHMARK(char_stream_advance)->Arg(100)->Arg(1000)->Arg(10000);

}  // namespace base
