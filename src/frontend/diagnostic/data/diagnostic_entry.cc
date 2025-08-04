// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/diagnostic_entry.h"

#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"

namespace diagnostic {

DiagnosticEntry::DiagnosticEntry(Header&& header, Labels&& labels)
    : header_(std::move(header)), labels_(std::move(labels)) {}

}  // namespace diagnostic
