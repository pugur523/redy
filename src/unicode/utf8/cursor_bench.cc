// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>

#include "benchmark/benchmark.h"
#include "core/base/file_manager.h"
#include "unicode/utf8/cursor.h"
#include "unicode/utf8/file_manager.h"

namespace unicode {

void utf8_cursor_init(benchmark::State& state) {
  std::u8string input(state.range(0), 'a');
  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Cursor cursor;
    cursor.init(&manager, id);
  }
  state.SetBytesProcessed(sizeof(Utf8Cursor) * state.iterations());
}
BENCHMARK(utf8_cursor_init)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_cursor_peek(benchmark::State& state) {
  std::u8string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Cursor cursor;
    cursor.init(&manager, id);
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(cursor.peek());
    }
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_cursor_peek)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_cursor_next(benchmark::State& state) {
  std::u8string input(state.range(0), 'a');
  std::size_t input_size = input.size();
  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Cursor cursor;
    cursor.init(&manager, id);
    while (!cursor.eof()) {
      benchmark::DoNotOptimize(cursor.next());
    }
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_cursor_next)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_cursor_next_utf8(benchmark::State& state) {
  // alternating "あ😊"
  std::u8string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += u8"あ😊";
  }
  std::size_t input_size = input.size();

  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Cursor cursor;
    cursor.init(&manager, id);
    while (!cursor.eof()) {
      benchmark::DoNotOptimize(cursor.next());
    }
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_cursor_next_utf8)->Arg(1024)->Arg(4096)->Arg(16384);

void utf8_cursor_peek_utf8(benchmark::State& state) {
  std::u8string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += u8"あ😊";
  }
  std::size_t input_size = input.size();

  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Cursor cursor;
    cursor.init(&manager, id);
    for (int64_t i = 0; i < state.range(0); ++i) {
      benchmark::DoNotOptimize(cursor.peek());
    }
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_cursor_peek_utf8)->Arg(1024)->Arg(4096)->Arg(16384);

// random mixed (ascii + breakline + utf8) next
void utf8_cursor_mixed_next(benchmark::State& state) {
  std::u8string input;
  for (int i = 0; i < state.range(0); ++i) {
    input += u8"a\nあ😊z\n";
  }
  std::size_t input_size = input.size();

  Utf8FileManager manager;
  Utf8FileId id = manager.register_virtual_file(std::move(input));
  for (auto _ : state) {
    Utf8Cursor cursor;
    cursor.init(&manager, id);
    while (!cursor.eof()) {
      cursor.next();
    }
  }
  state.SetBytesProcessed(input_size * state.iterations());
}
BENCHMARK(utf8_cursor_mixed_next)->Arg(1024)->Arg(4096)->Arg(16384);

}  // namespace unicode
