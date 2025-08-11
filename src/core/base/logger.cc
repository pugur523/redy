// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/base/logger.h"

#include "build/build_flag.h"
#include "femtolog/logger.h"
#include "femtolog/sinks/file_sink.h"
#include "femtolog/sinks/stdout_sink.h"

namespace core {

// extern
// femtolog::Logger& glog = []() -> femtolog::Logger& {
//   femtolog::Logger& logger = femtolog::Logger::global_logger();
//   return logger;
// }();

// extern
femtolog::Logger& glog = femtolog::Logger::global_logger();

void register_glog() {
  femtolog::FemtologOptions options{
      .spsc_queue_size = 1024 * 2,              // 2KiB
      .backend_format_buffer_size = 1024 * 2,   // 2KiB
      .backend_dequeue_buffer_size = 1024 * 2,  // 2KiB
  };
  glog.init(options);
  glog.register_sink<femtolog::StdoutSink<>>();
  glog.register_sink<femtolog::FileSink>();
  glog.start_worker();

#if IS_DEBUG
  glog.level("debug");
#else
  glog.level("info");
#endif
}

}  // namespace core
