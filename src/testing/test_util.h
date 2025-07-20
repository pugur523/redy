// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef TESTING_TEST_UTIL_H_
#define TESTING_TEST_UTIL_H_

#include <string>

#include "core/base/file_util.h"

inline const std::string& test_dir() {
  static std::string temp_test_dir = [] {
    std::string d = core::join_path(core::exe_dir(), "test");
    if (!core::dir_exists(d.c_str())) {
      core::create_directories(d.c_str());
    }
    return d;
  }();
  return temp_test_dir;
}

#endif  // TESTING_TEST_UTIL_H_
