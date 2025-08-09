// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_STYLE_UTIL_H_
#define CORE_BASE_STYLE_UTIL_H_

#include <array>
#include <cstdint>

#include "core/base/core_export.h"

namespace core {

// base styles
enum class Style : uint8_t {
  kReset = 0,
  kBold = 1 << 0,       // 1
  kDim = 1 << 1,        // 2
  kItalic = 1 << 2,     // 4
  kUnderline = 1 << 3,  // 8
  kBlink = 1 << 4,      // 16
  kReverse = 1 << 5,    // 32
  kHidden = 1 << 6,     // 64
  kStrike = 1 << 7,     // 128

  // alias
  kBoldUnderline = kBold | kUnderline,
  kBoldItalic = kBold | kItalic,

  kDefault = kReset,
};

inline constexpr Style operator|(Style a, Style b) {
  return static_cast<Style>(static_cast<uint16_t>(a) |
                            static_cast<uint16_t>(b));
}

inline constexpr Style operator&(Style a, Style b) {
  return static_cast<Style>(static_cast<uint16_t>(a) &
                            static_cast<uint16_t>(b));
}

inline constexpr Style operator^(Style a, Style b) {
  return static_cast<Style>(static_cast<uint16_t>(a) ^
                            static_cast<uint16_t>(b));
}

inline constexpr Style operator~(Style a) {
  return static_cast<Style>(~static_cast<uint16_t>(a));
}

inline constexpr Style& operator|=(Style& a, Style b) {
  return a = a | b;
}

inline constexpr Style& operator&=(Style& a, Style b) {
  return a = a & b;
}

inline constexpr Style& operator^=(Style& a, Style b) {
  return a = a ^ b;
}

inline constexpr bool has_style(Style styles, Style style) {
  return (styles & style) != Style::kDefault;
}

// foreground colours
enum class Colour : uint8_t {
  kBlack = 0,
  kRed = 1,
  kGreen = 2,
  kYellow = 3,
  kBlue = 4,
  kMagenta = 5,
  kCyan = 6,
  kWhite = 7,
  kGray = 8,
  kBrightRed = 9,
  kBrightGreen = 10,
  kBrightYellow = 11,
  kBrightBlue = 12,
  kBrightMagenta = 13,
  kBrightCyan = 14,
  kBrightWhite = 15,

  kDefault = 255,
};

// background colours
enum class BgColour : uint8_t {
  kBlack = 0,
  kRed = 1,
  kGreen = 2,
  kYellow = 3,
  kBlue = 4,
  kMagenta = 5,
  kCyan = 6,
  kWhite = 7,
  kGray = 8,
  kBrightRed = 9,
  kBrightGreen = 10,
  kBrightYellow = 11,
  kBrightBlue = 12,
  kBrightMagenta = 13,
  kBrightCyan = 14,
  kBrightWhite = 15,

  kDefault = 255,
};

}  // namespace core

#endif  // CORE_BASE_STYLE_UTIL_H_
