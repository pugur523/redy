// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_DIAGNOSTICS_STACK_TRACE_ENTRY_H_
#define CORE_DIAGNOSTICS_STACK_TRACE_ENTRY_H_

#include <array>
#include <cstddef>

#include "core/base/core_export.h"

namespace core {

static constexpr std::size_t kAddressStrLength = 32;
static constexpr std::size_t kFunctionStrLength = 256;
static constexpr std::size_t kFileStrLength = 512;
struct CORE_EXPORT StackTraceEntry {
 public:
  StackTraceEntry() {
    address.fill('\0');
    function.fill('\0');
    file.fill('\0');
  }

  void to_string(char* out_buf, std::size_t out_buf_size) const;

  std::array<char, kAddressStrLength> address;
  std::array<char, kFunctionStrLength> function;
  std::array<char, kFileStrLength> file;
  std::size_t index = 0;
  std::size_t line = 0;
  std::size_t offset = 0;
  bool use_index : 1 = false;

  static constexpr std::size_t kIndexAlignLength = 7;
  static constexpr std::size_t kAddressAlignLength = 20;
  static constexpr std::size_t kFunctionAlignLength = 30;
  static constexpr const char* kUnknownFunction = "(unknown function)";
};

}  // namespace core

#endif  // CORE_DIAGNOSTICS_STACK_TRACE_ENTRY_H_
