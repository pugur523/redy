// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "app/app.h"

#include "app/cli_handler.h"
#include "app/init_handler.h"
#include "app/preinit_handler.h"
#include "core/base/logger.h"

namespace app {

int start(int argc, char** argv) {
  on_preinit();
  all_initialize();

  handle_arguments(argc, argv);

  const std::size_t dropped = core::glog.dropped_count();
  if (dropped > 0) {
    core::glog.warn_ref<"logging dropped count {}\n">(dropped);
  }

  return 0;
}

}  // namespace app
