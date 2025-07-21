// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_LABEL_H_
#define FRONTEND_DIAGNOSTIC_DATA_LABEL_H_

#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "core/base/source_location.h"
#include "core/base/source_range.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/annotation.h"

namespace diagnostic {

class DIAGNOSTIC_EXPORT Label {
 public:
  using Annotations = std::vector<Annotation>;

  Label(core::FileId file_id,
        core::SourceRange&& range,
        Annotations&& annotations,
        std::string&& message);

  ~Label() = default;

  Label(const Label&) = delete;
  Label& operator=(const Label&) = delete;

  Label(Label&&) = default;
  Label& operator=(Label&&) = default;

  inline void add_annotation(std::string&& message, AnnotationKind kind) {
    annotations_.emplace_back(std::move(message), kind);
  }

  inline const core::SourceRange& range() const { return range_; }
  inline const Annotations& annotations() const { return annotations_; }
  inline const std::string& message() const { return message_; }
  inline core::FileId file_id() const { return file_id_; }

 private:
  core::SourceRange range_;
  Annotations annotations_;
  std::string message_;
  core::FileId file_id_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_LABEL_H_
