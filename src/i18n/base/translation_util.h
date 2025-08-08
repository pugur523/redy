// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef I18N_BASE_TRANSLATION_UTIL_H_
#define I18N_BASE_TRANSLATION_UTIL_H_

#include <cstddef>
#include <string_view>

namespace i18n {

template <std::size_t N>
struct FixedString {
  char data[N + 1]{};
  std::size_t size = N;

  consteval FixedString(const char (&str)[N + 1]) {  // NOLINT
    for (std::size_t i = 0; i < N; ++i) {
      data[i] = str[i];
    }
    data[N] = '\0';
  }

  consteval std::string_view view() const {
    return std::string_view{data, size};
  }

  consteval const char* c_str() const { return data; }
};

template <std::size_t N>
FixedString(const char (&)[N]) -> FixedString<N - 1>;

template <FixedString a, FixedString b>
consteval bool str_equal() {
  constexpr std::size_t n = a.size;
  constexpr std::size_t m = b.size;
  constexpr std::string_view a_view = a.view();
  constexpr std::string_view b_view = b.view();
  if constexpr (n != m) {
    return false;
  } else {
    std::size_t i = 0;
    while (i < n && a_view.at(i) != '\0' && a_view.at(i) == b_view.at(i)) {
      ++i;
    }
    return i == n;
  }
}

// set this to the maximum number of placeholders in each message in
// //src/build/resources/i18n/*/*.toml
constexpr const std::size_t kMaxFormatArgsCount = 3;

using FormatArgs = std::array<std::string_view, kMaxFormatArgsCount>;

}  // namespace i18n

#endif  // I18N_BASE_TRANSLATION_UTIL_H_
