// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_ERROR_SOURCE_ERROR_H_
#define FRONTEND_DIAGNOSTIC_DATA_ERROR_SOURCE_ERROR_H_

#include <string_view>
#include <utility>

#include "core/base/file_manager.h"
#include "core/base/source_range.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"

namespace diagnostic {

// does not manages file id but only manages range of source so we must keep
// info about correspond file externaly to get the content of the source which
// contains error
struct DIAGNOSTIC_EXPORT SourceError {
  core::SourceRange range;
  DiagnosticId diag_id;
  Severity severity;

  static inline constexpr SourceError create(
      std::size_t start_line,
      std::size_t start_column,
      std::size_t length,
      DiagnosticId diag_id,
      Severity severity = Severity::kError) {
    return create({start_line, start_column, length}, diag_id, severity);
  }

  static inline constexpr SourceError create(
      core::SourceRange range,
      DiagnosticId diag_id,
      Severity severity = Severity::kError) {
    return SourceError{
        .range = range,
        .diag_id = diag_id,
        .severity = severity,
    };
  }

  DiagnosticEntry convert_to_entry() &&;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_ERROR_SOURCE_ERROR_H_
