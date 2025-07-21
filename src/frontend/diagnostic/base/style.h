// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_BASE_STYLE_H_
#define FRONTEND_DIAGNOSTIC_BASE_STYLE_H_

#include <cstdint>

#include "core/base/string_util.h"

namespace diagnostic {

enum class Style : uint8_t {
  kUnknown = 0,
  kReset = 1,
  kBold = 2,
  kRed = 3,
  kYellow = 4,
  kGreen = 5,
  kCyan = 6,
  kMagenta = 7,
  kGray = 8,
  kBrightRed = 9,
  kBrightGreen = 10,
  kBrightCyan = 11,
};

inline const char* style_to_string(Style style) {
  switch (style) {
    case Style::kUnknown: return "[unknown_style] ";
    case Style::kReset: return core::kReset;
    case Style::kBold: return core::kBold;
    case Style::kRed: return core::kRed;
    case Style::kYellow: return core::kYellow;
    case Style::kGreen: return core::kGreen;
    case Style::kCyan: return core::kCyan;
    case Style::kMagenta: return core::kMagenta;
    case Style::kGray: return core::kGray;
    case Style::kBrightRed: return core::kBrightRed;
    case Style::kBrightGreen: return core::kBrightGreen;
    case Style::kBrightCyan: return core::kBrightCyan;
    default: return "[invalid_style] ";
  }
}

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_BASE_STYLE_H_
