// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_ANNOTATION_H_
#define FRONTEND_DIAGNOSTIC_DATA_ANNOTATION_H_

#include <cstdint>
#include <utility>

#include "core/cli/ansi/style_util.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "i18n/base/data/translation_key.h"

namespace diagnostic {

enum class AnnotationSeverity : uint8_t {
  kUnknown = 0,
  kNote = 1,
  kSuggestion = 2,
  kHelp = 3,
  kWarn = 4,
  kError = 5,
  kFatal = 6,
};

inline const char* annotation_severity_to_string(AnnotationSeverity severity) {
  using Severity = AnnotationSeverity;
  switch (severity) {
    case Severity::kUnknown: return "unknown";
    case Severity::kNote: return "note";
    case Severity::kSuggestion: return "suggestion";
    case Severity::kHelp: return "help";
    case Severity::kWarn: return "warn";
    case Severity::kError: return "error";
    case Severity::kFatal: return "fatal";
    default: return "invalid";
  }
}

inline core::Colour annotation_severity_to_colour(AnnotationSeverity severity) {
  using Severity = AnnotationSeverity;
  switch (severity) {
    case Severity::kUnknown: return core::Colour::kDefault;
    case Severity::kNote: return core::Colour::kCyan;
    case Severity::kSuggestion: return core::Colour::kBrightCyan;
    case Severity::kHelp: return core::Colour::kBlue;
    case Severity::kWarn: return core::Colour::kYellow;
    case Severity::kError: return core::Colour::kRed;
    case Severity::kFatal: return core::Colour::kMagenta;
  }
}

class DIAGNOSTIC_EXPORT Annotation {
 public:
  constexpr Annotation(AnnotationSeverity severity,
                       i18n::TranslationKey tr_key,
                       std::initializer_list<std::string_view> args)
      : severity_(severity), tr_key_(tr_key), args_count_(args.size()) {
    std::size_t i = 0;
    for (const auto& arg : args) {
      if (i < format_args_.size()) {
        format_args_[i++] = arg;
      }
    }
  }

  ~Annotation() = default;

  Annotation(const Annotation&) = delete;
  Annotation& operator=(const Annotation&) = delete;

  Annotation(Annotation&&) = default;
  Annotation& operator=(Annotation&&) = default;

  inline const i18n::FormatArgs& format_args() const { return format_args_; }
  inline AnnotationSeverity severity() const { return severity_; }
  inline i18n::TranslationKey message_tr_key() const { return tr_key_; }
  inline uint8_t args_count() const { return args_count_; }
  inline bool should_format() const { return args_count_ > 0; }

 private:
  i18n::FormatArgs format_args_{};
  AnnotationSeverity severity_ = AnnotationSeverity::kUnknown;
  i18n::TranslationKey tr_key_ = i18n::TranslationKey::kUnknown;
  uint8_t args_count_ = 0;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_ANNOTATION_H_
