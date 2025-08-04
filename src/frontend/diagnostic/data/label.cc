// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/label.h"

#include <string>
#include <utility>

#include "core/base/file_manager.h"

namespace diagnostic {

Label::Label(core::FileId file_id,
             const core::SourceRange& range,
             Annotations&& annotations,
             std::string&& message)
    : range_(range),
      annotations_(std::move(annotations)),
      message_(std::move(message)),
      file_id_(file_id) {}

}  // namespace diagnostic
