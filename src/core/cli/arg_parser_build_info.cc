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
)">(BUILD_NAME, BUILD_VERSION, BUILD_TYPE, BUILD_PLATFORM, BUILD_ARCH,
    TARGET_PLATFORM, TARGET_ARCH, TARGET_BITS, BUILD_COMPILER, exe_dir(),
    BUILD_TIME, BUILD_GIT_COMMIT_HASH);
}

}  // namespace core
