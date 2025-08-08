// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/entry_builder.h"

#include <string>
#include <utility>

#include "core/check.h"

namespace diagnostic {

EntryBuilder::EntryBuilder(Severity severity, DiagnosticId id)
    : header_(severity, id) {}

}  // namespace diagnostic
