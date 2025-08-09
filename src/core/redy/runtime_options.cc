// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/redy/runtime_options.h"

#include <string>

#include "core/redy/build_type.h"

namespace core {

std::string RuntimeOptions::to_string(std::size_t padding_size) {
  std::string result;
  std::string pad = '\n' + std::string(padding_size, ' ');

  constexpr std::size_t kPredictedMaxOptionsStringSize = 1024;
  result.reserve(kPredictedMaxOptionsStringSize);

  if (!config_file.empty()) {
    result.append("config file: ").append(config_file);
    result.append(pad);
  }

  result.append("verbose: ").append(verbose ? "true" : "false").append(pad);

  const char* type_str = build_type_to_string(build_type);
  result.append("build type: ").append(type_str).push_back('\n');

  result.append("sub command: ").append(sub_command).push_back('\n');

  return result;
}

}  // namespace core
