// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_CLI_ANSI_PROGRESS_BAR_H_
#define CORE_CLI_ANSI_PROGRESS_BAR_H_

#include <algorithm>
#include <chrono>
#include <deque>
#include <string>

#include "core/base/core_export.h"
#include "core/check.h"

namespace core {

class CORE_EXPORT ProgressBar {
 public:
  explicit ProgressBar(std::size_t width = 40, bool enable_color = true)
      : width_(width),
        start_time_(std::chrono::steady_clock::now()),
        enable_color_(enable_color) {}

  ~ProgressBar() = default;

  ProgressBar(const ProgressBar&) = delete;
  ProgressBar& operator=(const ProgressBar&) = delete;

  ProgressBar(ProgressBar&&) = default;
  ProgressBar& operator=(ProgressBar&&) = default;

  // update progress in the range from 0 to 1
  [[nodiscard]] std::string update(double progress,
                                   const std::string& prefix = "",
                                   bool remove_line = false);

  [[nodiscard]] inline std::string finish(
      const std::string& msg = "Completed") {
    return update(1.0, msg, true);
  }

 private:
  std::string create_bar(double progress) const;

  inline static std::string format_time(double total_seconds);

  std::size_t width_;
  std::chrono::steady_clock::time_point start_time_;
  std::deque<double> eta_history_;
  bool enable_color_ : 1 = true;
};

}  // namespace core

#endif  // CORE_CLI_ANSI_PROGRESS_BAR_H_
