// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_REDY_RUNTIME_OPTIONS_H_
#define CORE_REDY_RUNTIME_OPTIONS_H_

#include <memory>
#include <string>

#include "core/base/core_export.h"
#include "core/base/logger.h"
#include "core/redy/build_type.h"

namespace core {

struct CORE_EXPORT RuntimeOptions {
 public:
  ~RuntimeOptions() = default;

  RuntimeOptions(const RuntimeOptions&) = delete;
  RuntimeOptions& operator=(const RuntimeOptions&) = delete;

  RuntimeOptions(RuntimeOptions&&) = default;
  RuntimeOptions& operator=(RuntimeOptions&&) = default;

  std::string to_string(std::size_t padding_size = 0);
  inline void print_all() { core::glog.info<"{}\n">(to_string()); }
  inline static std::unique_ptr<RuntimeOptions> create() {
    return std::unique_ptr<RuntimeOptions>(new RuntimeOptions());
  }

  // options
  std::string config_file = "redy_config.toml";
  std::string sub_command = "compile";
  bool verbose = false;
  BuildType build_type = BuildType::kDebug;

 private:
  RuntimeOptions() = default;
};

}  // namespace core

#endif  // CORE_REDY_RUNTIME_OPTIONS_H_
