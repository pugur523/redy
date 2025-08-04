// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_STRING_UTIL_H_
#define CORE_BASE_STRING_UTIL_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <format>
#include <queue>
#include <string>
#include <string_view>
#include <utility>

#include "core/base/core_export.h"

namespace core {

[[nodiscard]] CORE_EXPORT std::string encode_escape(std::string_view input);
[[nodiscard]] CORE_EXPORT std::string encode_escape(const char* s,
                                                    std::size_t len);
[[nodiscard]] CORE_EXPORT std::string decode_escape(std::string_view input);
[[nodiscard]] CORE_EXPORT std::string decode_escape(const char* s,
                                                    std::size_t len);

[[nodiscard]] inline constexpr bool is_valid_escape_sequence(char backslash,
                                                             char next) {
  if (backslash != '\\') {
    return false;
  }

  switch (next) {
    // simple escape sequences
    case 'n':
    case 'r':
    case 't':
    case 'v':
    case 'b':
    case 'a':
    case 'f':
    case '\\':
    case '\'':
    case '\"':
    case '?':

    // hexadecimal \xHH
    case 'x':  // need to validate digits after 'x' elsewhere

    // unicode escape sequences
    case 'u':
    case 'U':

    // octal escape \[0-7]{1,3}
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7': return true;

    default: return false;
  }
}

[[nodiscard]] inline bool is_valid_unicode_escape(std::string_view input) {
  if (input.size() != 5 && input.size() != 9) {
    return false;
  } else if (input.starts_with("u")) {
    return input.size() == 5 &&
           std::all_of(input.begin() + 1, input.end(), ::isxdigit);
  } else if (input.starts_with("U")) {
    return input.size() == 9 &&
           std::all_of(input.begin() + 1, input.end(), ::isxdigit);
  } else {
    return false;
  }
}

[[nodiscard]] inline bool is_valid_hex_escape(std::string_view input) {
  if (input.empty() || input[0] != 'x') {
    return false;
  }
  return std::all_of(input.begin() + 1, input.end(), ::isxdigit);
}

[[nodiscard]] inline bool is_valid_octal_escape(std::string_view input) {
  if (input.empty() || input.size() > 3) {
    return false;
  }
  return std::all_of(input.begin(), input.end(),
                     [](char c) { return c >= '0' && c <= '7'; });
}

[[nodiscard]] inline constexpr bool is_ascii_char(char c) {
  return 0 <= c && c <= 127;
}

[[nodiscard]] inline constexpr bool is_lower_ascii_char(char c) {
  return 'a' <= c && c <= 'z';
}

[[nodiscard]] inline constexpr bool is_upper_ascii_char(char c) {
  return 'A' <= c && c <= 'Z';
}

[[nodiscard]] inline constexpr bool is_ascii_alphabet(char c) {
  return is_lower_ascii_char(c) || is_upper_ascii_char(c);
}

[[nodiscard]] inline constexpr bool is_ascii_digit(char c) {
  return '0' <= c && c <= '9';
}

[[nodiscard]] inline constexpr bool is_ascii_binary_digit(char c) {
  return c == '0' || c == '1';
}

[[nodiscard]] inline constexpr bool is_ascii_octal_digit(char c) {
  return '0' <= c && c <= '7';
}

[[nodiscard]] inline constexpr bool is_ascii_hex_digit(char c) {
  return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') ||
         ('A' <= c && c <= 'F');
}

[[nodiscard]] inline constexpr bool is_utf8_start_byte(unsigned char byte) {
  return (byte & 0xC0) != 0x80;
}

[[nodiscard]] inline constexpr uint8_t utf8_codepoint_length(
    unsigned char byte) {
  if ((byte & 0x80) == 0x00) {
    return 1;
  } else if ((byte & 0xE0) == 0xC0) {
    return 2;
  } else if ((byte & 0xF0) == 0xE0) {
    return 3;
  } else if ((byte & 0xF8) == 0xF0) {
    return 4;
  } else {
    return 0;
  }
}

[[nodiscard]] inline constexpr char to_lower(char c) {
  return is_upper_ascii_char(c) ? (c | 0x20) : c;
}
CORE_EXPORT void to_lower(char* input, std::size_t len);
CORE_EXPORT void to_lower(char* input);
CORE_EXPORT void to_lower(std::string* input);
[[nodiscard]] CORE_EXPORT std::string to_lower(const std::string& input);

[[nodiscard]] constexpr char to_upper(char c) {
  return is_lower_ascii_char(c) ? (c & ~0x20) : c;
}
CORE_EXPORT void to_upper(char* input, std::size_t len);
CORE_EXPORT void to_upper(char* input);
CORE_EXPORT void to_upper(std::string* input);
[[nodiscard]] CORE_EXPORT std::string to_upper(const std::string& input);

[[nodiscard]] CORE_EXPORT std::size_t utf8_char_length(unsigned char lead);
[[nodiscard]] CORE_EXPORT std::string utf8_truncate(const std::string& input,
                                                    std::size_t max_chars);
[[nodiscard]] CORE_EXPORT std::queue<std::string> split_string(
    const std::string& input,
    const std::string& delimiter);
[[nodiscard]] CORE_EXPORT std::string remove_bracket(
    const std::string& input,
    std::size_t max_nest_size = 32);
[[nodiscard]] CORE_EXPORT std::size_t safe_strlen(const char* str);

CORE_EXPORT void format_address_safe(uintptr_t addr,
                                     char* buffer_start,
                                     std::size_t buffer_size);

CORE_EXPORT void padding(char*& cursor,
                         const char* const end,
                         std::size_t current_len,
                         std::size_t align_len);

CORE_EXPORT std::size_t write_raw(char*& dest,
                                  const char* source,
                                  std::size_t len);

template <typename... Args>
std::size_t write_format(char*& cursor,
                         const char* const end,
                         std::format_string<Args...> fmt,
                         Args&&... args) {
  std::ptrdiff_t remaining = end - cursor;
  if (remaining <= 0) {
    return 0;
  }

  auto result =
      std::format_to_n(cursor, static_cast<std::size_t>(remaining - 1), fmt,
                       std::forward<Args>(args)...);
  std::size_t written = result.out - cursor;

  cursor += written;
  *cursor = '\0';
  return written;
}

inline void write_bytes(char*& cursor, const void* data, std::size_t size) {
  std::memcpy(cursor, data, size);
  cursor += size;
}

inline void read_bytes(const char*& cursor, void* data, std::size_t size) {
  std::memcpy(data, cursor, size);
  cursor += size;
}

inline bool starts_with(const std::string& input,
                        const std::string& prefix,
                        std::size_t index = 0) {
  return input.size() >= prefix.size() &&
         input.compare(index, prefix.size(), prefix) == 0;
}

template <typename T>
struct is_ostreamable {
 private:
  template <typename U>
  static auto test(int)  // NOLINT
      -> decltype(std::declval<std::ostream&>() << std::declval<U>(),
                  std::true_type{});

  template <typename>
  static std::false_type test(...);

 public:
  static constexpr bool value = decltype(test<T>(0))::value;
};

template <typename T>
inline constexpr bool is_ostreamable_v = is_ostreamable<T>::value;

inline constexpr const char* kReset = "\033[0m";
inline constexpr const char* kBold = "\033[1m";
inline constexpr const char* kRed = "\033[31m";
inline constexpr const char* kYellow = "\033[33m";
inline constexpr const char* kGreen = "\033[32m";
inline constexpr const char* kCyan = "\033[36m";
inline constexpr const char* kBlue = "\033[34m";
inline constexpr const char* kMagenta = "\033[35m";
inline constexpr const char* kGray = "\033[90m";
inline constexpr const char* kBrightRed = "\033[91m";
inline constexpr const char* kBrightGreen = "\033[92m";
inline constexpr const char* kBrightCyan = "\033[96m";

}  // namespace core

#endif  // CORE_BASE_STRING_UTIL_H_
