// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_DATA_STRING_ARENA_H_
#define FRONTEND_BASE_DATA_STRING_ARENA_H_

#include <cstddef>
#include <utility>
#include <vector>

#include "frontend/base/base_export.h"

namespace base {

class BASE_EXPORT StringArena {
 public:
  StringArena() = default;
  ~StringArena() = default;

  StringArena(const StringArena&) = delete;
  StringArena& operator=(const StringArena&) = delete;

  StringArena(StringArena&&) noexcept = default;
  StringArena& operator=(StringArena&&) noexcept = default;

  // allocate and copy string into arena
  std::size_t alloc(std::string_view s);

  inline constexpr const std::vector<char>& buffer() const { return buffer_; }
  inline constexpr void reserve(std::size_t n) { buffer_.reserve(n); }
  inline constexpr void resize(std::size_t n) { buffer_.resize(n); }
  inline constexpr std::size_t size() const { return offsets_.size(); }

  inline constexpr std::string_view operator[](std::size_t id) const {
    if (id == 0 || id > offsets_.size()) {
      return {};
    }
    return std::string_view(buffer_.data() + offsets_[id]);
  }

 private:
  std::vector<char> buffer_;
  std::vector<uint32_t> offsets_;
};

}  // namespace base

#endif  // FRONTEND_BASE_DATA_STRING_ARENA_H_
