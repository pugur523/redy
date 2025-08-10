// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/diagnostics/terminate_handler.h"

#include <iostream>

#include "core/base/logger.h"
#include "core/diagnostics/stack_trace.h"

namespace core {

void terminate_handler() {
  core::glog.error<
      "program terminated unexpectedly\n"
      "stack trace (most recent call last):\n{}\n">(
      stack_trace_from_current_context());
  core::glog.stop_worker();
  std::exit(EXIT_FAILURE);
}

void register_terminate_handler() {
  std::set_terminate(terminate_handler);
}

}  // namespace core
