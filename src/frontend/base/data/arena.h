// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_DATA_ARENA_H_
#define FRONTEND_BASE_DATA_ARENA_H_

#include <cstddef>
#include <utility>
#include <vector>

#include "frontend/base/base_export.h"

namespace base {

// arena allocation utility class for data oriented design
// this is currently just a wrapper of std::vector, but it has some useful
// helper methods
template <typename T, typename Id = std::size_t>
class BASE_EXPORT Arena {
 public:
  Arena() = default;
  ~Arena() = default;

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  Arena(Arena&&) noexcept = default;
  Arena& operator=(Arena&&) noexcept = default;

  Id alloc(T&& value) {
    buffer_.emplace_back(std::move(value));
    return static_cast<Id>(buffer_.size() - 1);
  }

  inline constexpr const std::vector<T>& buffer() const { return buffer_; }
  inline constexpr void reserve(std::size_t n) { buffer_.reserve(n); }
  inline constexpr void resize(std::size_t n) { buffer_.resize(n); }
  inline constexpr std::size_t size() const { return buffer_.size(); }

  inline constexpr T& operator[](Id id) { return buffer_[id]; }
  inline constexpr const T& operator[](Id id) const { return buffer_[id]; }

 private:
  std::vector<T> buffer_;
};

}  // namespace base

#endif  // FRONTEND_BASE_DATA_ARENA_H_
