// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_ENTRY_BUILDER_H_
#define FRONTEND_DIAGNOSTIC_DATA_ENTRY_BUILDER_H_

#include <string_view>
#include <utility>
#include <vector>

#include "core/base/source_range.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/annotation.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/header.h"
#include "frontend/diagnostic/data/label.h"
#include "i18n/base/data/translation_key.h"

namespace i18n {
class Translator;
}

namespace diagnostic {

class DIAGNOSTIC_EXPORT EntryBuilder {
 public:
  using Labels = std::vector<Label>;
  using Annotations = std::vector<Annotation>;

  EntryBuilder(Severity severity, DiagnosticId id);

  ~EntryBuilder() = default;

  EntryBuilder(const EntryBuilder&) = delete;
  EntryBuilder& operator=(const EntryBuilder&) = delete;

  EntryBuilder(EntryBuilder&&) = default;
  EntryBuilder& operator=(EntryBuilder&&) = default;

  inline EntryBuilder& label(
      unicode::Utf8FileId file_id,
      std::size_t line,
      std::size_t column,
      std::size_t length,
      i18n::TranslationKey message_tr_key,
      LabelMarkerType marker_type = LabelMarkerType::kLine,
      std::initializer_list<std::string_view> args = {}) {
    Label label(file_id, {line, column, length}, {}, message_tr_key,
                marker_type, args);
    labels_.emplace_back(std::move(label));
    return *this;
  }

  inline EntryBuilder& label(
      unicode::Utf8FileId file_id,
      const core::SourceRange& range,
      i18n::TranslationKey message_tr_key,
      LabelMarkerType marker_type = LabelMarkerType::kLine,
      std::initializer_list<std::string_view> args = {}) {
    Label label(file_id, range, {}, message_tr_key, marker_type, args);
    labels_.emplace_back(std::move(label));
    return *this;
  }

  inline EntryBuilder& annotation(
      AnnotationSeverity severity,
      i18n::TranslationKey message_tr_key,
      std::initializer_list<std::string_view> args = {}) {
    DCHECK(!labels_.empty());
    labels_.back().add_annotation(severity, message_tr_key, args);
    return *this;
  }

  DiagnosticEntry build() && {
    return DiagnosticEntry(std::move(header_), std::move(labels_));
  }

 private:
  Header header_;
  Labels labels_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_ENTRY_BUILDER_H_
