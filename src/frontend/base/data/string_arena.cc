// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/data/string_arena.h"

namespace base {

std::size_t StringArena::alloc(std::string_view s) {
  if (s.empty()) {
    return 0;
  }
  const uint32_t offset = static_cast<uint32_t>(buffer_.size());
  const std::size_t index = offsets_.size();
  buffer_.insert(buffer_.end(), s.begin(), s.end());
  buffer_.push_back('\0');
  offsets_.push_back(offset);
  return index;  // start from 0
}

}  // namespace base
