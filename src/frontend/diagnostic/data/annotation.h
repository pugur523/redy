// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_ANNOTATION_H_
#define FRONTEND_DIAGNOSTIC_DATA_ANNOTATION_H_

#include <string>

#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/base/style.h"

namespace diagnostic {

enum class AnnotationKind : uint8_t {
  kUnknown = 0,
  kNote = 1,
  kSuggestion = 2,
  kHelp = 3,
  kWarn = 4,
  kError = 5,
  kFatal = 6,
};

inline const char* annotation_kind_to_string(AnnotationKind kind) {
  using Kind = AnnotationKind;
  switch (kind) {
    case Kind::kUnknown: return "unknown";
    case Kind::kNote: return "note";
    case Kind::kSuggestion: return "suggestion";
    case Kind::kHelp: return "help";
    case Kind::kWarn: return "warn";
    case Kind::kError: return "error";
    case Kind::kFatal: return "fatal";
    default: return "invalid";
  }
}

inline Style annotation_kind_to_style(AnnotationKind kind) {
  using Kind = AnnotationKind;
  switch (kind) {
    case Kind::kUnknown: return Style::kUnknown;
    case Kind::kNote: return Style::kCyan;
    case Kind::kSuggestion: return Style::kBrightRed;
    case Kind::kHelp: return Style::kGreen;
    case Kind::kWarn: return Style::kYellow;
    case Kind::kError: return Style::kRed;
    case Kind::kFatal: return Style::kMagenta;
    default: return Style::kUnknown;
  }
}

class DIAGNOSTIC_EXPORT Annotation {
 public:
  explicit Annotation(std::string&& message, AnnotationKind kind);

  ~Annotation() = default;

  Annotation(const Annotation&) = delete;
  Annotation& operator=(const Annotation&) = delete;

  Annotation(Annotation&&) = default;
  Annotation& operator=(Annotation&&) = default;

  const std::string& message() const { return message_; }
  AnnotationKind kind() const { return kind_; }

 private:
  std::string message_;
  AnnotationKind kind_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_ANNOTATION_H_

