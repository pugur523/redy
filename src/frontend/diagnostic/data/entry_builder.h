// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_ENTRY_BUILDER_H_
#define FRONTEND_DIAGNOSTIC_DATA_ENTRY_BUILDER_H_

#include <string>
#include <vector>

#include "core/base/source_range.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/header.h"
#include "frontend/diagnostic/data/label.h"

namespace diagnostic {

class DIAGNOSTIC_EXPORT EntryBuilder {
 public:
  using Labels = std::vector<Label>;
  using Annotations = std::vector<Annotation>;

  EntryBuilder(Severity severity, DiagnosticId id, std::string&& header_msg);

  ~EntryBuilder() = default;

  EntryBuilder(const EntryBuilder&) = delete;
  EntryBuilder& operator=(const EntryBuilder&) = delete;

  EntryBuilder(EntryBuilder&&) = default;
  EntryBuilder& operator=(EntryBuilder&&) = default;

  EntryBuilder& label(core::FileId file_Id,
                      const core::SourceRange& range,
                      std::string&& message);

  EntryBuilder& label(core::FileId file_id,
                      std::size_t line,
                      std::size_t column,
                      std::size_t length,
                      std::string&& message);

  EntryBuilder& annotation(std::string&& message, AnnotationKind kind);

  DiagnosticEntry build();

 private:
  Header header_;
  Labels labels_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_ENTRY_BUILDER_H_
