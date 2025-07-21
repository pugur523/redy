// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_OPTIONS_H_
#define FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_OPTIONS_H_

#include <cstdint>

#include "frontend/diagnostic/base/diagnostic_export.h"

namespace diagnostic {

enum class DiagnosticOutputFormat : uint8_t {
  kUnknown = 0,
  kClassic = 1,
  kShort = 2,
  kJson = 3,
};

struct DIAGNOSTIC_EXPORT DiagnosticOptions {
  DiagnosticOutputFormat output_format = DiagnosticOutputFormat::kClassic;
  bool colorize : 1 = true;
  bool show_notes : 1 = true;
  bool show_help : 1 = true;
  bool show_source_snippet : 1 = true;
  bool use_unicode : 1 = true;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_OPTIONS_H_
