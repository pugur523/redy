// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_LOGGER_H_
#define CORE_BASE_LOGGER_H_

#include "build/build_flag.h"
#include "core/base/core_export.h"
#include "femtolog/logger.h"

namespace core {

CORE_EXPORT extern femtolog::Logger& glog;

CORE_EXPORT void register_glog();

#if IS_DEBUG
#define DLOG(level, format, ...) ::core::glog.level<format>(__VA_ARGS__)
#else
#define DLOG(level, format, ...)
#endif

}  // namespace core

#endif  // CORE_BASE_LOGGER_H_
