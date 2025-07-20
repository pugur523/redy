// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "app/app.h"

#include "app/cli_handler.h"
#include "app/init_handler.h"
#include "app/preinit_handler.h"

namespace app {

int start(int argc, char** argv) {
  on_preinit();
  all_initialize();

  handle_arguments(argc, argv);

  return 0;
}

}  // namespace app
