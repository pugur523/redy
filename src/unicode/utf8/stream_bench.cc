// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "core/base/file_manager.h"
#include "unicode/utf8/file_manager.h"
#include "unicode/utf8/stream.h"

namespace unicode {

void utf8_stream_init(benchmark::State& state) {
  std::u8string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Stream stream;
    stream.init(&manager, id);
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_stream_init)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_stream_peek(benchmark::State& state) {
  std::u8string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(&manager, id);
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(stream.peek());
    }
    stream.reset();
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_stream_peek)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_stream_next(benchmark::State& state) {
  std::u8string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(&manager, id);
  for (auto _ : state) {
    while (!stream.eof()) {
      benchmark::DoNotOptimize(stream.next());
    }
    stream.reset();
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_stream_next)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_stream_next_utf8(benchmark::State& state) {
  // alternating "あ😊"
  std::u8string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += u8"あ😊";
  }
  std::size_t input_size = input.size();

  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(&manager, id);
  for (auto _ : state) {
    while (!stream.eof()) {
      benchmark::DoNotOptimize(stream.next());
    }
    stream.reset();
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_stream_next_utf8)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_stream_peek_utf8(benchmark::State& state) {
  std::u8string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += u8"あ😊";
  }
  std::size_t input_size = input.size();

  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(&manager, id);
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(stream.peek());
    }
    stream.reset();
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_stream_peek_utf8)->Arg(1024)->Arg(4096)->Arg(16384);

// random mixed (ascii + breakline + utf8) next
void utf8_stream_mixed_next(benchmark::State& state) {
  std::u8string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += u8"a\nあ😊z\n";
  }
  std::size_t input_size = input.size();

  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  Utf8Stream stream;
  stream.init(&manager, id);
  for (auto _ : state) {
    while (!stream.eof()) {
      benchmark::DoNotOptimize(stream.next());
    }
    stream.reset();
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_stream_mixed_next)->Arg(1024)->Arg(4096)->Arg(16384);

}  // namespace unicode
