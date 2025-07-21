// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_EXPORT_H_
#define FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_EXPORT_H_

#include "build/component_export.h"

namespace diagnostic {

#define DIAGNOSTIC_EXPORT COMPONENT_EXPORT(DIAGNOSTIC)

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_EXPORT_H_
