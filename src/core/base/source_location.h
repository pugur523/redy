// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_SOURCE_LOCATION_H_
#define CORE_BASE_SOURCE_LOCATION_H_

#include "core/base/core_export.h"
#include "core/base/file_manager.h"
#include "core/base/vec.h"

namespace core {

class CORE_EXPORT SourceLocation {
 public:
  constexpr SourceLocation(std::size_t line, std::size_t column)
      : internal_vec_({line, column}) {}

  ~SourceLocation() = default;

  SourceLocation(const SourceLocation&) = default;
  SourceLocation& operator=(const SourceLocation&) = default;

  SourceLocation(SourceLocation&&) = default;
  SourceLocation& operator=(SourceLocation&&) = default;

  inline constexpr std::size_t line() const { return internal_vec_[0]; }
  inline constexpr std::size_t column() const { return internal_vec_[1]; }

 private:
  core::Vec2<std::size_t> internal_vec_;
};

}  // namespace core

#endif  // CORE_BASE_SOURCE_LOCATION_H_
