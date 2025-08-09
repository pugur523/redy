// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <string>
#include <vector>

#include "benchmark/benchmark.h"
#include "unicode/utf8/decoder.h"

namespace unicode {

namespace {
std::string generate_ascii(size_t n) {
  return std::string(n, 'A');  // 0x41
}

std::string generate_two_byte(size_t n) {
  // u+00A9 copyright sign (0xC2 0xA9)
  std::string s;
  s.reserve(n * 2);
  for (size_t i = 0; i < n; ++i) {
    s.push_back(static_cast<char>(0xC2));
    s.push_back(static_cast<char>(0xA9));
  }
  return s;
}

std::string generate_three_byte(size_t n) {
  // u+65E5 '日' (0xE6 0x97 0xA5)
  std::string s;
  s.reserve(n * 3);
  for (size_t i = 0; i < n; ++i) {
    s.push_back(static_cast<char>(0xE6));
    s.push_back(static_cast<char>(0x97));
    s.push_back(static_cast<char>(0xA5));
  }
  return s;
}

std::string generate_four_byte(size_t n) {
  // u+1F600 😀 (0xF0 0x9F 0x98 0x80)
  std::string s;
  s.reserve(n * 4);
  for (size_t i = 0; i < n; ++i) {
    s.push_back(static_cast<char>(0xF0));
    s.push_back(static_cast<char>(0x9F));
    s.push_back(static_cast<char>(0x98));
    s.push_back(static_cast<char>(0x80));
  }
  return s;
}

std::string generate_mixed(size_t n) {
  std::string s;
  s.reserve(n * 3);
  for (size_t i = 0; i < n; ++i) {
    switch (i % 4) {
      case 0: s += "A"; break;                 // ascii
      case 1: s += "\xC2\xA9"; break;          // 2b
      case 2: s += "\xE6\x97\xA5"; break;      // 3b
      case 3: s += "\xF0\x9F\x98\x80"; break;  // 4b
    }
  }
  return s;
}

std::string generate_invalid(size_t n) {
  // continuation byte only (invalid UTF-8)
  return std::string(n, static_cast<char>(0x80));
}

void decode_ascii(benchmark::State& state) {
  Utf8Decoder decoder;
  auto data = generate_ascii(state.range(0));
  for (auto _ : state) {
    const char8_t* ptr = reinterpret_cast<const char8_t*>(data.data());
    const char8_t* end = ptr + data.size();
    char32_t sum = 0;
    while (ptr < end) {
      auto [cp, len] = decoder.decode(ptr, static_cast<size_t>(end - ptr));
      sum += cp;
      ptr += len;
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(data.size() * sizeof(char8_t) * state.iterations());
}
BENCHMARK(decode_ascii)->Arg(1024)->Arg(4096)->Arg(16384);

void decode_two_byte(benchmark::State& state) {
  Utf8Decoder decoder;
  auto data = generate_two_byte(state.range(0));
  for (auto _ : state) {
    const char8_t* ptr = reinterpret_cast<const char8_t*>(data.data());
    const char8_t* end = ptr + data.size();
    char32_t sum = 0;
    while (ptr < end) {
      auto [cp, len] = decoder.decode(ptr, static_cast<size_t>(end - ptr));
      sum += cp;
      ptr += len;
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(data.size() * sizeof(char8_t) * state.iterations());
}
BENCHMARK(decode_two_byte)->Arg(1024)->Arg(4096)->Arg(16384);

void decode_three_byte(benchmark::State& state) {
  Utf8Decoder decoder;
  auto data = generate_three_byte(state.range(0));
  for (auto _ : state) {
    const char8_t* ptr = reinterpret_cast<const char8_t*>(data.data());
    const char8_t* end = ptr + data.size();
    char32_t sum = 0;
    while (ptr < end) {
      auto [cp, len] = decoder.decode(ptr, static_cast<size_t>(end - ptr));
      sum += cp;
      ptr += len;
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(data.size() * sizeof(char8_t) * state.iterations());
}
BENCHMARK(decode_three_byte)->Arg(1024)->Arg(4096)->Arg(16384);

void decode_four_byte(benchmark::State& state) {
  Utf8Decoder decoder;
  auto data = generate_four_byte(state.range(0));
  for (auto _ : state) {
    const char8_t* ptr = reinterpret_cast<const char8_t*>(data.data());
    const char8_t* end = ptr + data.size();
    char32_t sum = 0;
    while (ptr < end) {
      auto [cp, len] = decoder.decode(ptr, static_cast<size_t>(end - ptr));
      sum += cp;
      ptr += len;
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(data.size() * sizeof(char32_t) * state.iterations());
}
BENCHMARK(decode_four_byte)->Arg(1024)->Arg(4096)->Arg(16384);

void decode_mixed(benchmark::State& state) {
  Utf8Decoder decoder;
  auto data = generate_mixed(state.range(0));
  for (auto _ : state) {
    const char8_t* ptr = reinterpret_cast<const char8_t*>(data.data());
    const char8_t* end = ptr + data.size();
    char32_t sum = 0;
    while (ptr < end) {
      auto [cp, len] = decoder.decode(ptr, static_cast<size_t>(end - ptr));
      sum += cp;
      ptr += len;
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(data.size() * sizeof(char8_t) * state.iterations());
}
BENCHMARK(decode_mixed)->Arg(1024)->Arg(4096)->Arg(16384);

void decode_invalid(benchmark::State& state) {
  Utf8Decoder decoder;
  auto data = generate_invalid(state.range(0));
  for (auto _ : state) {
    const char8_t* ptr = reinterpret_cast<const char8_t*>(data.data());
    const char8_t* end = ptr + data.size();
    char32_t sum = 0;
    while (ptr < end) {
      auto [cp, len] = decoder.decode(ptr, static_cast<size_t>(end - ptr));
      sum += cp;
      ptr += len;
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetBytesProcessed(data.size() * sizeof(char8_t) * state.iterations());
}
BENCHMARK(decode_invalid)->Arg(1024)->Arg(4096)->Arg(16384);

}  // namespace

}  // namespace unicode
