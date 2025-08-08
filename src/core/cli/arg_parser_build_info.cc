// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "build/build_config.h"
#include "core/base/file_util.h"
#include "core/base/logger.h"
#include "core/cli/arg_parser.h"

namespace core {

// static
void ArgumentParser::print_version() {
  core::glog.info<R"(
    {} version {} ({})
    Build Platform: {} - {}
    Target Platform: {} - {}
    Target Bits: {}
    Build Compiler: {}
    Installed Directory: {}
    Build Time: {}
    Commit Hash: {}
)">(build::kBuildName, build::kBuildVersion, build::kBuildType,
    build::kBuildPlatform, build::kBuildArch, build::kTargetPlatform,
    build::kTargetArch, build::kTargetBits, build::kBuildCompiler, exe_dir(),
    build::kBuildTime, build::kBuildGitCommitHash);
}

}  // namespace core
