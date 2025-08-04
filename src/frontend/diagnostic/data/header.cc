// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/header.h"

#include <string>
#include <utility>

#include "core/base/file_manager.h"
#include "frontend/diagnostic/data/diagnostic_id.h"

namespace diagnostic {

Header::Header(Severity severity, DiagnosticId diag_id, std::string&& message)
    : message_(std::move(message)), severity_(severity), diag_id_(diag_id) {}

}  // namespace diagnostic
