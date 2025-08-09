// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_SEVERITY_H_
#define FRONTEND_DIAGNOSTIC_DATA_SEVERITY_H_

#include <cstdint>

#include "core/base/style_util.h"
#include "i18n/base/data/translation_key.h"

namespace diagnostic {

enum class Severity : uint8_t {
  kUnknown = 0,
  kFatal = 1,
  kError = 2,
  kWarn = 3,
  kInfo = 4,
  kDebug = 5,
  kTrace = 6,
};

inline constexpr i18n::TranslationKey severity_to_tr_key(Severity severity) {
  using Tk = i18n::TranslationKey;
  switch (severity) {
    case Severity::kUnknown: return Tk::kDiagnosticUnknown;
    case Severity::kTrace: return Tk::kDiagnosticSeverityTrace;
    case Severity::kDebug: return Tk::kDiagnosticSeverityDebug;
    case Severity::kInfo: return Tk::kDiagnosticSeverityInfo;
    case Severity::kWarn: return Tk::kDiagnosticSeverityWarn;
    case Severity::kError: return Tk::kDiagnosticSeverityError;
    case Severity::kFatal: return Tk::kDiagnosticSeverityFatal;
  }
}

inline constexpr core::Colour severity_to_colour(Severity severity) {
  switch (severity) {
    case Severity::kUnknown: return core::Colour::kDefault;
    case Severity::kFatal: return core::Colour::kMagenta;
    case Severity::kError: return core::Colour::kRed;
    case Severity::kWarn: return core::Colour::kYellow;
    case Severity::kInfo: return core::Colour::kGreen;
    case Severity::kDebug: return core::Colour::kCyan;
    case Severity::kTrace: return core::Colour::kGray;
  }
}

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_SEVERITY_H_
