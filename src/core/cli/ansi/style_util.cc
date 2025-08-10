// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/cli/ansi/style_util.h"

#include "build/build_flag.h"

#if IS_WINDOWS
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

namespace core {

bool check_ansi_sequence_available() {
#if IS_WINDOWS
  // check if GetStdHandle() returns terminal handle
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  if (hOut == INVALID_HANDLE_VALUE) {
    return false;
  }

  DWORD dwMode = 0;
  if (!GetConsoleMode(hOut, &dwMode)) {
    return false;
  }

  // enable virtual terminal processing
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if (!SetConsoleMode(hOut, dwMode)) {
    // cannot use ansi sequence if fail to enable virtual terminal
    return false;
  }
  return true;
#else
  return isatty(STDOUT_FILENO);
#endif
}

}  // namespace core
