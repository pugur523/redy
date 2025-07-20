// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/base/source_range.h"

#include <utility>

namespace core {

SourceRange::SourceRange(const SourceLocation& start, const SourceLocation& end)
    : start_(start), end_(end) {}

SourceRange::SourceRange(std::size_t line,
                         std::size_t column,
                         std::size_t length,
                         FileId file_id)
    : SourceRange(SourceLocation(line, column, file_id),
                  SourceLocation(line, column + length, file_id)) {}

}  // namespace core
