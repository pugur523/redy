// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/check.h"

#include <exception>
#include <iostream>

#include "core/base/logger.h"

namespace core {

CheckFailureStream::CheckFailureStream(const char* type,
                                       const char* file,
                                       int line,
                                       const char* condition,
                                       const char* message)
    : type_(type),
      file_(file),
      line_(line),
      condition_(condition),
      message_(message) {}

std::ostream& CheckFailureStream::stream() {
  glog.error_ref<"{} failed: \"{}\" at {}:{}\n{}\n">(type_, condition_, file_,
                                                     line_, message_);
  return std::cerr;
}

CheckFailureStream::~CheckFailureStream() {
  std::terminate();
}

}  // namespace core
