// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/base/style_builder.h"

#include <charconv>
#include <string>
#include <string_view>

#include "build/build_flag.h"

#if IS_WINDOWS
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace core {

namespace {

constexpr const char* kResetCode = "\033[0m";
constexpr const char* kFgRgbPrefix = "\033[38;2;";
constexpr const char* kBgRgbPrefix = "\033[48;2;";
constexpr const char kRgbSuffix = 'm';
constexpr const char kSemicolon = ';';

constexpr const std::size_t kStyleCodeLength = 4;
constexpr const std::size_t kResetCodeLength = kStyleCodeLength;
constexpr const std::size_t kRgbCodeLength = 20;

inline constexpr std::array<const char*, 8> kStyleCodes = {
    "\033[1m", "\033[2m", "\033[3m", "\033[4m",
    "\033[5m", "\033[7m", "\033[8m", "\033[9m",
};

inline constexpr std::array<const char*, 16> kColourCodes = {
    "\033[30m", "\033[31m", "\033[32m", "\033[33m", "\033[34m", "\033[35m",
    "\033[36m", "\033[37m", "\033[90m", "\033[91m", "\033[92m", "\033[93m",
    "\033[94m", "\033[95m", "\033[96m", "\033[97m",
};

inline constexpr std::array<const char*, 16> kBgColourCodes = {
    "\033[40m",  "\033[41m",  "\033[42m",  "\033[43m",
    "\033[44m",  "\033[45m",  "\033[46m",  "\033[47m",
    "\033[100m", "\033[101m", "\033[102m", "\033[103m",
    "\033[104m", "\033[105m", "\033[106m", "\033[107m",
};

// used for 0~255 only
inline char* uint8_to_chars(char* buffer, uint8_t value) {
  if (value >= 100) {
    *buffer++ = '0' + (value / 100);
    value %= 100;
    *buffer++ = '0' + (value / 10);
    *buffer++ = '0' + (value % 10);
  } else if (value >= 10) {
    *buffer++ = '0' + (value / 10);
    *buffer++ = '0' + (value % 10);
  } else {
    *buffer++ = '0' + value;
  }
  return buffer;
}

void append_rgb_sequence(std::string* result,
                         std::string_view prefix,
                         const uint8_t rgb[3]) {
  constexpr std::size_t kBufSize = 32;
  char buffer[kBufSize];
  char* pos = buffer;

  // prefix
  for (char c : prefix) {
    *pos++ = c;
  }

  // r
  pos = uint8_to_chars(pos, rgb[0]);
  *pos++ = kSemicolon;

  // g
  pos = uint8_to_chars(pos, rgb[1]);
  *pos++ = kSemicolon;

  // b
  pos = uint8_to_chars(pos, rgb[2]);

  // suffix
  *pos++ = kRgbSuffix;

  result->append(buffer, pos - buffer);
}

}  // namespace

const bool can_use_ansi_sequence = [] {
#if IS_WINDOWS
  // check if GetStdHandle() returns terminal handle
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return false;
  }

  // enable virtual terminal processing
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    // cannot use ansi sequence if fail to enable virtual terminal
    return false;
  }
  return true;
#else
  return isatty(STDOUT_FILENO);
#endif
}();

std::string StyleBuilder::build(std::string_view text) const {
  std::string result;

  if (!should_build_) {
    result = text;
    return result;
  }

  // size prediction
  std::size_t estimated_size = text.size() + kResetCodeLength;
  uint16_t style_bits = static_cast<uint16_t>(styles_);
  int style_count = 0;
  for (int i = 0; i < 8; ++i) {
    if (style_bits & (1 << i)) {
      style_count++;
    }
  }
  estimated_size += style_count * kStyleCodeLength;
  if (use_fg_rgb_) {
    estimated_size += kRgbCodeLength;
  } else if (fg_ != Colour::kDefault) {
    estimated_size += kStyleCodeLength;
  }
  if (use_bg_rgb_) {
    estimated_size += kRgbCodeLength;
  } else if (bg_ != BgColour::kDefault) {
    estimated_size += kStyleCodeLength;
  }
  result.reserve(estimated_size);

  // style
  if (styles_ != Style::kDefault) {
    uint16_t style_bits = static_cast<uint16_t>(styles_);
    for (int i = 0; i < 8; ++i) {
      if (style_bits & (1 << i)) {
        result.append(kStyleCodes[i]);
      }
    }
  }

  // foreground
  if (use_fg_rgb_) {
    append_rgb_sequence(&result, kFgRgbPrefix, fg_rgb_);
  } else if (fg_ != Colour::kDefault) {
    result.append(kColourCodes[static_cast<uint8_t>(fg_)]);
  }

  // background
  if (use_bg_rgb_) {
    append_rgb_sequence(&result, kBgRgbPrefix, bg_rgb_);
  } else if (bg_ != BgColour::kDefault) {
    result.append(kBgColourCodes[static_cast<uint8_t>(bg_)]);
  }

  // text + reset
  result.append(text);
  result.append(kResetCode);

  return result;
}

}  // namespace core

