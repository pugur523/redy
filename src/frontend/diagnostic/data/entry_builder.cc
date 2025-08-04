// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/entry_builder.h"

#include <string>
#include <utility>

#include "core/check.h"

namespace diagnostic {

EntryBuilder::EntryBuilder(Severity severity,
                           DiagnosticId id,
                           std::string&& header_msg)
    : header_(severity, id, std::move(header_msg)) {}

EntryBuilder& EntryBuilder::label(core::FileId file_id,
                                  const core::SourceRange& range,
                                  std::string&& message) {
  Annotations annotations;
  labels_.emplace_back(file_id, range, std::move(annotations),
                       std::move(message));
  return *this;
}

EntryBuilder& EntryBuilder::label(core::FileId file_id,
                                  std::size_t line,
                                  std::size_t column,
                                  std::size_t length,
                                  std::string&& message) {
  return label(file_id, core::SourceRange(line, column, length),
               std::move(message));
}

EntryBuilder& EntryBuilder::annotation(std::string&& message,
                                       AnnotationKind kind) {
  DCHECK(!labels_.empty());
  labels_.back().add_annotation(std::move(message), kind);
  return *this;
}

DiagnosticEntry EntryBuilder::build() {
  return DiagnosticEntry(std::move(header_), std::move(labels_));
}

}  // namespace diagnostic
