// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/diagnostic_entry.h"

#include <algorithm>
#include <utility>

#include "core/base/file_manager.h"

namespace diagnostic {

DiagnosticEntry::DiagnosticEntry(Header&& header, Labels&& labels)
    : header_(std::move(header)), labels_(std::move(labels)) {}

DiagnosticEntry::Labels& DiagnosticEntry::sort_labels() {
  std::sort(labels_.begin(), labels_.end(), [](const Label& a, const Label& b) {
    if (a.file_id() != b.file_id()) {
      return a.file_id() < b.file_id();
    }
    return a.range().start().line() < b.range().start().line();
  });
  return labels_;
}

}  // namespace diagnostic
