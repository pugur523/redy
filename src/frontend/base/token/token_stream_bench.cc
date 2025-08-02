// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <utility>
#include <vector>

#include "benchmark/benchmark.h"
#include "frontend/base/token/token.h"
#include "frontend/base/token/token_stream.h"

namespace base {

namespace {

void token_stream_advance(benchmark::State& state) {
  core::FileManager manager;
  core::FileId file_id = manager.add_virtual_file(std::string(1000, 'x'));

  std::vector<Token> tokens;

  tokens.reserve(1001);
  for (int i = 0; i < 1000; ++i) {
    tokens.emplace_back(TokenKind::kLiteralNumeric, file_id, 1, i, 1);
  }
  tokens.emplace_back(TokenKind::kEof, file_id, 1, 1000, 0);
  TokenStream stream(std::move(tokens), &manager);

  for (auto _ : state) {
    while (!stream.eof()) {
      const Token& token = stream.advance();
      benchmark::DoNotOptimize(token.kind());
    }
    stream.rewind(0);
  }
}
BENCHMARK(token_stream_advance);

static void token_stream_peak(benchmark::State& state) {
  core::FileManager manager;
  core::FileId file_id = manager.add_virtual_file(std::string(1000, '3'));
  std::vector<Token> tokens;
  tokens.reserve(1001);
  for (int i = 0; i < 1000; ++i) {
    tokens.emplace_back(TokenKind::kLiteralNumeric, file_id, 1, i, 1);
  }
  tokens.emplace_back(TokenKind::kEof, file_id, 1, 1000, 0);
  TokenStream stream(std::move(tokens), &manager);

  for (auto _ : state) {
    for (std::size_t i = 0; i < tokens.size(); ++i) {
      const Token& token = stream.peek();
      benchmark::DoNotOptimize(token.kind());
      stream.advance();
    }
    stream.rewind(0);
  }
}
BENCHMARK(token_stream_peak);

}  // namespace

}  // namespace base
