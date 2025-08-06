// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "core/base/file_manager.h"
#include "unicode/base/utf8_stream.h"

namespace unicode {

void utf8_stream_peek(benchmark::State& state) {
  std::string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(manager.file(id));
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(stream.peek());
    }
  }
  state.SetBytesProcessed(input_size * sizeof(char) * state.iterations());
}
BENCHMARK(utf8_stream_peek)->Arg(100)->Arg(1000)->Arg(10000);

void utf8_stream_advance(benchmark::State& state) {
  std::string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(manager.file(id));
  for (auto _ : state) {
    // this code will be optimized (maybe unroll loop related)
    // while (!stream.eof()) {
    //   stream.advance();
    //   benchmark::DoNotOptimize(stream.peek());
    // }
    for (int64_t i = 0; i < state.range(0); ++i) {
      stream.advance();
      benchmark::DoNotOptimize(stream.peek());
    }
  }
  state.SetBytesProcessed(input_size * sizeof(char) * state.iterations());
}
BENCHMARK(utf8_stream_advance)->Arg(100)->Arg(1000)->Arg(10000);

void utf8_stream_advance_utf8(benchmark::State& state) {
  // alternating "あ😊"
  std::string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += "あ😊";
  }
  std::size_t input_size = input.size();

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Stream stream;
    stream.init(manager.file(id));
    while (!stream.eof()) {
      stream.advance();
    }
  }
  state.SetBytesProcessed(input_size * sizeof(char32_t) * state.iterations());
}
BENCHMARK(utf8_stream_advance_utf8)->Arg(100)->Arg(1000)->Arg(10000);

void utf8_stream_peek_utf8(benchmark::State& state) {
  std::string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += "あ😊";
  }
  std::size_t input_size = input.size();

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(manager.file(id));
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(stream.peek());
    }
  }
  state.SetBytesProcessed(input_size * sizeof(char32_t) * state.iterations());
}
BENCHMARK(utf8_stream_peek_utf8)->Arg(100)->Arg(1000)->Arg(10000);

void utf8_stream_rewind(benchmark::State& state) {
  std::string input(state.range(0), 'x');
  std::size_t input_size = input.size();
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Stream stream;
    stream.init(manager.file(id));
    for (int64_t i = 0; i < state.range(0); ++i) {
      stream.advance();
    }
    for (int64_t i = 0; i < state.range(0); ++i) {
      stream.rewind();
    }
    benchmark::ClobberMemory();
  }
  state.SetBytesProcessed(input_size * sizeof(char) * state.iterations());
}
BENCHMARK(utf8_stream_rewind)->Arg(100)->Arg(1000)->Arg(10000);

// random mixed (ascii + breakline + utf8) advance
void utf8_stream_mixed_advance(benchmark::State& state) {
  std::string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += "a\nあ😊z\n";
  }
  std::size_t input_size = input.size();

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Stream stream;
    stream.init(manager.file(id));
    while (!stream.eof()) {
      stream.advance();
    }
  }
  state.SetBytesProcessed(input_size * sizeof(char32_t) * state.iterations());
}
BENCHMARK(utf8_stream_mixed_advance)->Arg(100)->Arg(1000)->Arg(10000);

}  // namespace unicode
