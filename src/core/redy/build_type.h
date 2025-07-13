// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_REDY_BUILD_TYPE_H_
#define CORE_REDY_BUILD_TYPE_H_

#include <cstdint>
#include <string>

#include "core/base/core_export.h"
#include "core/base/string_util.h"

namespace core {

enum class BuildType : uint8_t {
  kDebug = 0,
  kRelease = 1,
  kReleaseWithDebugInfo = 2,
  kMinimumSizeRelease = 3,
  kUnknown = 4,
};

CORE_EXPORT constexpr const char* build_type_to_string(BuildType type) {
  switch (type) {
    case (BuildType::kDebug): return "debug";
    case (BuildType::kRelease): return "release";
    case (BuildType::kReleaseWithDebugInfo): return "release_with_debug_info";
    case (BuildType::kMinimumSizeRelease): return "minimum_size_release";
    default: return "unknown";
  }
}

CORE_EXPORT inline BuildType build_type_from_string(
    const std::string& type_str) {
  const std::string lowered = to_lower(type_str);
  if (lowered == "debug") {
    return BuildType::kDebug;
  } else if (lowered == "release") {
    return BuildType::kRelease;
  } else if (lowered == "release_with_debug_info") {
    return BuildType::kReleaseWithDebugInfo;
  } else if (lowered == "minimum_size_release") {
    return BuildType::kMinimumSizeRelease;
  } else {
    return BuildType::kUnknown;
  }
}

}  // namespace core

#endif  // CORE_REDY_BUILD_TYPE_H_
