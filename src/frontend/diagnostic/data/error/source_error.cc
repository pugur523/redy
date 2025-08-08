// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/error/source_error.h"

#include <utility>

#include "frontend/diagnostic/data/entry_builder.h"

namespace diagnostic {

DiagnosticEntry SourceError::convert_to_entry() && {
  return EntryBuilder(severity, diag_id).build();
}

}  // namespace diagnostic
