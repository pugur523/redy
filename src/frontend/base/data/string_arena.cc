// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/data/string_arena.h"

namespace base {

std::size_t StringArena::alloc(std::string_view s) {
  if (s.empty()) {
    return 0;
  }
  const std::size_t offset = buffer_.size();
  buffer_.insert(buffer_.end(), s.begin(), s.end());
  // imcompatible with cstring, but we only use string_view so not needed
  // buffer_.push_back('\0');
  string_infos_.emplace_back(offset, s.length());
  return string_infos_.size();  // start from 1
}

}  // namespace base
