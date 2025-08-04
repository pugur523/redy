// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "frontend/base/data/char_stream.h"

namespace base {

void char_stream_peek(benchmark::State& state) {
  std::string input(state.range(0), 'a');
  state.SetBytesProcessed(input.size() * sizeof(char));
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager, id);
  for (auto _ : state) {
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(cs.peek());
    }
  }
}
BENCHMARK(char_stream_peek)->Arg(100)->Arg(1000)->Arg(10000);

void char_stream_advance(benchmark::State& state) {
  std::string input(state.range(0), 'a');
  state.SetBytesProcessed(input.size() * sizeof(char));
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager, id);
  for (auto _ : state) {
    while (!cs.eof()) {
      cs.advance();
      benchmark::DoNotOptimize(cs.peek());
    }
  }
}
BENCHMARK(char_stream_advance)->Arg(100)->Arg(1000)->Arg(10000);

void char_stream_advance_codepoint_utf8(benchmark::State& state) {
  // alternating "あ😊"
  std::string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += "あ😊";
  }
  state.SetBytesProcessed(input.size() * sizeof(char32_t));

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  for (auto _ : state) {
    CharStream cs(&manager, id);
    while (!cs.eof()) {
      cs.advance_codepoint();
    }
  }
}
BENCHMARK(char_stream_advance_codepoint_utf8)->Arg(100)->Arg(1000)->Arg(10000);

// `peek_codepoint` hot loop
void char_stream_peek_codepoint(benchmark::State& state) {
  std::string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += "あ😊";
  }
  state.SetBytesProcessed(input.size() * sizeof(char32_t));

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  CharStream cs(&manager, id);
  for (auto _ : state) {
    std::size_t sum = 0;
    for (int64_t i = 0; i < state.range(0); ++i) {
      sum += cs.peek_codepoint();
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(char_stream_peek_codepoint)->Arg(100)->Arg(1000)->Arg(10000);

void char_stream_rewind(benchmark::State& state) {
  std::string input(state.range(0), 'x');
  state.SetBytesProcessed(input.size() * sizeof(char));
  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  for (auto _ : state) {
    CharStream cs(&manager, id);
    for (int64_t i = 0; i < state.range(0); ++i) {
      cs.advance();
    }
    for (int64_t i = 0; i < state.range(0); ++i) {
      cs.rewind();
    }
    benchmark::ClobberMemory();
  }
}
BENCHMARK(char_stream_rewind)->Arg(100)->Arg(1000)->Arg(10000);

// random mixed (ascii + breakline + utf8) advance_codepoint
void char_stream_mixed_advance(benchmark::State& state) {
  std::string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += "a\nあ😊z\n";
  }
  state.SetBytesProcessed(input.size() * sizeof(char32_t));

  core::FileManager manager;
  core::FileId id = manager.add_virtual_file(std::move(input));
  for (auto _ : state) {
    CharStream cs(&manager, id);
    while (!cs.eof()) {
      cs.advance_codepoint();
    }
  }
}
BENCHMARK(char_stream_mixed_advance)->Arg(100)->Arg(1000)->Arg(10000);

}  // namespace base
