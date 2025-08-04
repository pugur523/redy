// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_SOURCE_RANGE_H_
#define CORE_BASE_SOURCE_RANGE_H_

#include "core/base/core_export.h"
#include "core/base/file_manager.h"
#include "core/base/source_location.h"

namespace core {

class CORE_EXPORT SourceRange {
 public:
  constexpr SourceRange(const SourceLocation& start, const SourceLocation& end)
      : start_(start),
        length_(end.column() > start.column() ? end.column() - start.column()
                                              : 0) {}

  constexpr SourceRange(const SourceLocation& start, std::size_t length)
      : start_(start), length_(length) {}

  constexpr SourceRange(std::size_t line,
                        std::size_t column,
                        std::size_t length)
      : start_(line, column), length_(length) {}

  ~SourceRange() = default;

  SourceRange(const SourceRange&) = default;
  SourceRange& operator=(const SourceRange&) = default;

  SourceRange(SourceRange&&) = default;
  SourceRange& operator=(SourceRange&&) = default;

  inline constexpr const SourceLocation& start() const { return start_; }
  inline constexpr std::size_t length() const { return length_; }

  inline constexpr SourceLocation end() const {
    return SourceLocation(start_.line(), start_.column() + length_);
  }

  inline constexpr bool contains(const SourceLocation& loc) const {
    return loc.line() == start_.line() && loc.column() >= start_.column() &&
           loc.column() < start_.column() + length_;
  }

 private:
  SourceLocation start_;
  std::size_t length_;
};

}  // namespace core

#endif  // CORE_BASE_SOURCE_RANGE_H_
