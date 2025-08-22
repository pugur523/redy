// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ENTRY_H_
#define FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ENTRY_H_

#include <vector>

#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/header.h"
#include "frontend/diagnostic/data/label.h"

namespace diagnostic {

class DIAGNOSTIC_EXPORT DiagnosticEntry {
 public:
  using Labels = std::vector<Label>;

  DiagnosticEntry(Header&& header, Labels&& labels);

  ~DiagnosticEntry() = default;

  DiagnosticEntry(const DiagnosticEntry&) = delete;
  DiagnosticEntry& operator=(const DiagnosticEntry&) = delete;

  DiagnosticEntry(DiagnosticEntry&&) noexcept = default;
  DiagnosticEntry& operator=(DiagnosticEntry&&) noexcept = default;

  inline const Header& header() const { return header_; }
  inline const Labels& labels() const { return labels_; }

  Labels& sort_labels();

 private:
  Header header_;
  Labels labels_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ENTRY_H_
