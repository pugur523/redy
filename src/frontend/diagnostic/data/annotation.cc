// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/data/annotation.h"

#include <string>
#include <utility>

namespace diagnostic {

Annotation::Annotation(std::string&& message, AnnotationKind kind)
    : message_(std::move(message)), kind_(kind) {}

}  // namespace diagnostic
