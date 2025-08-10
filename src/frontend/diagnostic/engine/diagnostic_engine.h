// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_ENGINE_DIAGNOSTIC_ENGINE_H_
#define FRONTEND_DIAGNOSTIC_ENGINE_DIAGNOSTIC_ENGINE_H_

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "frontend/base/token/token.h"
#include "frontend/diagnostic/base/diagnostic_options.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/label.h"
#include "unicode/utf8/file_manager.h"

namespace i18n {
class Translator;
}

namespace diagnostic {

class DiagnosticEngine {
 public:
  using Entries = std::vector<DiagnosticEntry>;

  explicit DiagnosticEngine(const unicode::Utf8FileManager* file_manager,
                            const i18n::Translator* translator,
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

  inline void push(Entries&& entries) {
    for (auto&& entry : entries) {
      push(std::move(entry));
    }
  }

  inline void clear() { entries_.clear(); }

  std::string pop_and_format();
  std::string format_batch_and_clear();

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

  void format_label_body(const Label& label,
                         std::size_t line_number_width,
                         std::size_t current_line,
                         std::string* out_str) const;

  void format_labels(std::vector<Label>&& sorted_labels,
                     std::string* out_str) const;

  void format_header(const Header& header, std::string* out_str) const;

  void format_one(DiagnosticEntry&& entry, std::string* out_str) const;

  void format(Entries&& entries, std::string* out_str) const;

 private:
  // renders a specific source line Rith marker
  void render_source_line(std::string* out_str,
                          const Label& label,
                          std::string_view line,
                          std::size_t line_number_width) const;

  inline static void indent(std::string* out_str, std::size_t count = 1);

  template <typename T>
  inline static std::size_t itoa_to_buffer(T value,
                                           char* buffer,
                                           std::size_t buf_size);

  Entries entries_;
  const unicode::Utf8FileManager* file_manager_ = nullptr;
  const i18n::Translator* translator_ = nullptr;
  DiagnosticOptions options_;

  static constexpr const std::size_t kPredictedFormattedStrSize = 512;
  static constexpr const std::size_t kFormatBufSize = 256;
  static constexpr const std::size_t kItoaBufSize = 16;
};

// static
inline void DiagnosticEngine::indent(std::string* out_str, std::size_t count) {
  out_str->append(count * 2, ' ');
}

// static
template <typename T>
inline std::size_t DiagnosticEngine::itoa_to_buffer(T value,
                                                    char* buffer,
                                                    std::size_t buf_size) {
  static_assert(std::is_integral_v<T>, "itoa_to_buffer only supports integers");
  auto [ptr, ec] = std::to_chars(buffer, buffer + buf_size, value);
  DCHECK_EQ(ec, std::errc{});
  return static_cast<std::size_t>(ptr - buffer);
}

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_ENGINE_DIAGNOSTIC_ENGINE_H_
