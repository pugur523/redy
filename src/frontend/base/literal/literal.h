// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_LITERAL_LITERAL_H_
#define FRONTEND_BASE_LITERAL_LITERAL_H_

#include <cstdint>

namespace base {

enum class LiteralKind : uint8_t {
  kUnknown = 0,

  // numeric
  kDecimal = 1,
  kBinary = 2,
  kOctal = 3,
  kHexadecimal = 4,

  kString = 5,
  kCharacter = 6,
  kTrue = 7,
  kFalse = 8
};

}  // namespace base

#endif  // FRONTEND_BASE_LITERAL_LITERAL_H_
