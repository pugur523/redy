// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>
#include <vector>

#include "benchmark/benchmark.h"
#include "frontend/base/token/token.h"
#include "frontend/base/token/token_stream.h"
#include "unicode/utf8/file_manager.h"

namespace base {

namespace {

void token_stream_advance(benchmark::State& state) {
  unicode::Utf8FileManager manager;
  unicode::Utf8FileId file_id =
      manager.register_virtual_file(std::u8string(1000, 'x'));

  std::vector<Token> tokens;

  tokens.reserve(1001);
  for (int i = 0; i < 1000; ++i) {
    tokens.emplace_back(TokenKind::kDecimal, 1, i, 1);
  }
  tokens.emplace_back(TokenKind::kEof, 1, 1000, 0);
  TokenStream stream(std::move(tokens), &manager, file_id);

  for (auto _ : state) {
    while (!stream.eof()) {
      const Token& token = stream.next();
      benchmark::DoNotOptimize(token.kind());
    }
    stream.rewind(0);
  }

  state.SetBytesProcessed(1000 * state.iterations());
}
BENCHMARK(token_stream_advance);

void token_stream_peek(benchmark::State& state) {
  unicode::Utf8FileManager manager;
  unicode::Utf8FileId file_id =
      manager.register_virtual_file(std::u8string(1000, '3'));
  std::vector<Token> tokens;
  tokens.reserve(1001);
  for (int i = 0; i < 1000; ++i) {
    tokens.emplace_back(TokenKind::kDecimal, 1, i, 1);
  }
  // ensure do not advance to the eof token
  std::size_t tokens_size = tokens.size();
  tokens.emplace_back(TokenKind::kEof, 1, 1000, 0);
  TokenStream stream(std::move(tokens), &manager, file_id);

  for (auto _ : state) {
    for (std::size_t i = 0; i < tokens_size; ++i) {
      const Token& token = stream.peek();
      benchmark::DoNotOptimize(token.kind());
      stream.next();
    }
    stream.rewind(0);
  }

  state.SetBytesProcessed(1000 * state.iterations());
}
BENCHMARK(token_stream_peek);

}  // namespace

}  // namespace base
