// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_LABEL_H_
#define FRONTEND_DIAGNOSTIC_DATA_LABEL_H_

#include <format>
#include <initializer_list>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "core/base/source_location.h"
#include "core/base/source_range.h"
#include "core/cli/ansi/style_util.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/annotation.h"
#include "i18n/base/data/translation_key.h"
#include "unicode/utf8/file_manager.h"

namespace i18n {
class Translator;
}

namespace diagnostic {

enum class LabelMarkerType : uint8_t {
  kUnknown = 0,
  kLine = 1,          // `-----`
  kCircle = 2,        // `ooooo`
  kEqual = 3,         // `=====`
  kEmphasis = 4,      // `~~~~~`
  kCodeAddition = 5,  // `+++++`
  kCodeRemoval = 6,   // `xxxxx`
};

inline constexpr char label_maker_type_to_char(LabelMarkerType marker) {
  switch (marker) {
    case LabelMarkerType::kUnknown: return '@';
    case LabelMarkerType::kLine: return '-';
    case LabelMarkerType::kCircle: return 'o';
    case LabelMarkerType::kEqual: return '=';
    case LabelMarkerType::kEmphasis: return '~';
    case LabelMarkerType::kCodeAddition: return '+';
    case LabelMarkerType::kCodeRemoval: return 'x';
  }
}

inline constexpr core::Colour label_marker_type_to_colour(
    LabelMarkerType marker) {
  switch (marker) {
    case LabelMarkerType::kUnknown: return core::Colour::kGray;
    case LabelMarkerType::kLine: return core::Colour::kBlue;
    case LabelMarkerType::kCircle: return core::Colour::kBrightRed;
    case LabelMarkerType::kEqual: return core::Colour::kCyan;
    case LabelMarkerType::kEmphasis: return core::Colour::kRed;
    case LabelMarkerType::kCodeAddition: return core::Colour::kBrightGreen;
    case LabelMarkerType::kCodeRemoval: return core::Colour::kBrightCyan;
  }
}

class DIAGNOSTIC_EXPORT Label {
 public:
  using Annotations = std::vector<Annotation>;

  constexpr Label(unicode::Utf8FileId file_id,
                  const core::SourceRange& range,
                  Annotations&& annotations,
                  i18n::TranslationKey message_tr_key,
                  LabelMarkerType marker_type = LabelMarkerType::kLine,
                  std::initializer_list<std::string_view> args = {})
      : range_(range),
        annotations_(std::move(annotations)),
        file_id_(file_id),
        message_tr_key_(message_tr_key),
        marker_type_(marker_type),
        args_count_(args.size()) {
    std::size_t i = 0;
    for (const auto& arg : args) {
      if (i < format_args_.size()) {
        format_args_[i++] = arg;
      }
    }
  }

  ~Label() = default;

  Label(const Label&) = delete;
  Label& operator=(const Label&) = delete;

  Label(Label&&) = default;
  Label& operator=(Label&&) = default;

  inline void add_annotation(AnnotationSeverity severity,
                             i18n::TranslationKey message_tr_key,
                             std::initializer_list<std::string_view> args) {
    Annotation annotation(severity, message_tr_key, args);
    annotations_.emplace_back(std::move(annotation));
  }

  inline const i18n::FormatArgs& format_args() const { return format_args_; }
  inline const core::SourceRange& range() const { return range_; }
  inline const Annotations& annotations() const { return annotations_; }
  inline unicode::Utf8FileId file_id() const { return file_id_; }
  inline i18n::TranslationKey message_tr_key() const { return message_tr_key_; }
  inline LabelMarkerType marker_type() const { return marker_type_; }
  inline uint8_t args_count() const { return args_count_; }
  inline bool should_format() const { return args_count_ > 0; }

 private:
  i18n::FormatArgs format_args_{};
  core::SourceRange range_;
  Annotations annotations_;
  unicode::Utf8FileId file_id_ = core::kInvalidFileId;
  i18n::TranslationKey message_tr_key_ = i18n::TranslationKey::kUnknown;
  LabelMarkerType marker_type_ = LabelMarkerType::kUnknown;
  uint8_t args_count_ = 0;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_LABEL_H_
