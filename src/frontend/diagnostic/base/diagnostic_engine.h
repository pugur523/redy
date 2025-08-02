// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_ENGINE_H_
#define FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_ENGINE_H_

#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "frontend/base/token/token.h"
#include "frontend/diagnostic/base/diagnostic_options.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/label.h"

namespace diagnostic {

class DiagnosticEngine {
 public:
  using Entries = std::vector<DiagnosticEntry>;

  explicit DiagnosticEngine(const core::FileManager* file_manager,
                            DiagnosticOptions options);

  ~DiagnosticEngine() = default;

  DiagnosticEngine(const DiagnosticEngine&) = delete;
  DiagnosticEngine& operator=(const DiagnosticEngine&) = delete;

  DiagnosticEngine(DiagnosticEngine&&) = default;
  DiagnosticEngine& operator=(DiagnosticEngine&&) = default;

  inline const Entries& entries() const { return entries_; }

  inline void push(DiagnosticEntry&& entry) {
    entries_.push_back(std::move(entry));
  }

  inline void clear() { entries_.clear(); }

  std::string pop_and_format();
  std::string format_batch();

  void format_annotation(const Annotation& annotation,
                         std::size_t line_number_width,
                         std::string* out_str) const;
  void format_label_header(const Label& label,
                           std::size_t line_number_width,
                           const char* line_num_str,
                           std::size_t line_num_len,
                           const char* col_num_str,
                           std::size_t col_num_len,
                           std::string* out_str) const;
  void format_label(const Label& label,
                    std::size_t line_number_width,
                    const char* line_num_str,
                    std::size_t line_num_len,
                    std::string* out_str) const;
  void format(DiagnosticEntry&& entry, std::string* out_str) const;
  void format(Entries&& entries, std::string* out_str) const;

 private:
  inline const char* style(Style s) const {
    return options_.colorize ? style_to_string(s) : "";
  }

  // renders a specific source line Rith marker
  void render_source_line(const std::string& line,
                          std::size_t column_start,
                          std::size_t column_end,
                          const std::string& marker_msg,
                          std::size_t line_number_width,
                          std::string* out_str) const;

  static void indent(std::string* out_str, std::size_t count = 1);

  static std::size_t itoa_to_buffer(int value, char* buffer);

  Entries entries_;
  const core::FileManager* file_manager_ = nullptr;
  DiagnosticOptions options_;

  static constexpr const std::size_t kPredictedFormattedStrSize = 512;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_BASE_DIAGNOSTIC_ENGINE_H_
