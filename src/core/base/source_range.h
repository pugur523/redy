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
  SourceRange(const SourceLocation& start, const SourceLocation& end);

  SourceRange(std::size_t line,
              std::size_t column,
              std::size_t length,
              FileId file_id);

  ~SourceRange() = default;

  SourceRange(const SourceRange&) = delete;
  SourceRange& operator=(const SourceRange&) = delete;

  SourceRange(SourceRange&&) = default;
  SourceRange& operator=(SourceRange&&) = default;

  inline constexpr const SourceLocation& start() const { return start_; }
  inline constexpr const SourceLocation& end() const { return end_; }
  inline constexpr FileId file_id() { return start_.file_id(); }

 private:
  SourceLocation start_;
  SourceLocation end_;
};

}  // namespace core

#endif  // CORE_BASE_SOURCE_RANGE_H_
