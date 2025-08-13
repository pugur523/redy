// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_DATA_ARENA_H_
#define FRONTEND_BASE_DATA_ARENA_H_

#include <cstddef>
#include <vector>

#include "frontend/base/base_export.h"

namespace base {

template <typename T, typename Id = std::size_t>
class BASE_EXPORT Arena {
 public:
  Arena() = default;
  ~Arena() = default;

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  Arena(Arena&&) = default;
  Arena& operator=(Arena&&) = default;

  inline constexpr Id alloc(const T& value) {
    data_.emplace_back(value);
    return Id{static_cast<uint32_t>(data_.size() - 1)};
  }

  inline constexpr T& operator[](Id id) { return data_[id]; }
  inline constexpr const T& operator[](Id id) const { return data_[id]; }

  inline constexpr const std::vector<T>& data() const { return data_; }

 private:
  std::vector<T> data_;
};

}  // namespace base

#endif  // FRONTEND_BASE_DATA_ARENA_H_
