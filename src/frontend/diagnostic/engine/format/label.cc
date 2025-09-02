// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include "core/cli/ansi/style_builder.h"
#include "frontend/diagnostic/engine/diagnostic_engine.h"
#include "i18n/base/translator.h"

namespace diagnostic {

void DiagnosticEngine::format_label_header(const Label& label,
                                           std::size_t line_number_width,
                                           const char* line_num_str,
                                           std::size_t line_num_len,
                                           const char* col_num_str,
                                           std::size_t col_num_len,
                                           std::string* out_str) const {
  const unicode::Utf8File& file = file_manager_->loaded_file(label.file_id());
  const std::string_view file_name = file.file_name();

  core::StyleBuilder s;

  out_str->append(line_number_width, ' ');
  out_str->append("-> ");

  s.style(core::Style::kBold).color(core::Color::kBrightGreen);
  std::string file_info_buf;
  file_info_buf.reserve(file_name.size() + sizeof(":") + line_num_len +
                        sizeof(":") + col_num_len);
  file_info_buf.append(file_name);
  file_info_buf.append(":");
  file_info_buf.append(line_num_str, line_num_len);
  file_info_buf.append(":");
  file_info_buf.append(col_num_str, col_num_len);
  out_str->append(s.build(std::move(file_info_buf)));
  s.reset();

  out_str->append("\n");
  out_str->append(line_number_width, ' ');
  out_str->append(" |\n");
}

void DiagnosticEngine::format_label_body(const Label& label,
                                         std::size_t line_number_width,
                                         std::size_t current_line,
                                         std::string* out_str) const {
  const unicode::Utf8File& file = file_manager_->file(label.file_id());
  const core::SourceLocation& loc = label.range().start();
  const std::size_t label_line = loc.line();

  // if there are lines between current_line and label_line, print them
  // from (current_line + 1) .. (label_line - 1)
  if (label_line > current_line) {
    for (std::size_t i = current_line + 1; i < label_line; ++i) {
      // line number text for this pad line
      char pad_line_buf[kItoaBufSize];
      const std::size_t pad_line_len =
          itoa_to_buffer(i, pad_line_buf, kItoaBufSize);

      // pad so the digits are right-aligned in the line-number column
      out_str->append(line_number_width - pad_line_len, ' ');
      out_str->append(pad_line_buf, pad_line_len);

      // separator and the source line
      out_str->append(" | ");
      std::string_view pad_line = file.line(i);
      out_str->append(pad_line);
      out_str->append("\n");

      // trailing empty pipe line (match render_source_line's trailing " |"
      // line)
      out_str->append(line_number_width, ' ');
      out_str->append(" |\n");
    }
  }

  // now print the actual labeled line
  // compute line number string for this label
  char line_buf[kItoaBufSize];
  const std::size_t this_line_len =
      itoa_to_buffer(label_line, line_buf, kItoaBufSize);

  // pad line number manually, append only once
  out_str->append(line_number_width - this_line_len, ' ');
  out_str->append(line_buf, this_line_len);
  out_str->append(" | ");

  // fetch and render the source + marker for this label
  std::string_view line = file.line(label_line);
  render_source_line(out_str, label, line, line_number_width);

  for (const auto& ann : label.annotations()) {
    format_annotation(ann, line_number_width, out_str);
  }
}

void DiagnosticEngine::format_labels(std::vector<Label>&& sorted_labels,
                                     std::string* out_str) const {
  // resolve max line number of the labels for indent
  std::size_t max_line_number = 0;
  for (const Label& label : sorted_labels) {
    max_line_number = std::max(max_line_number, label.range().start().line());
  }

  char max_line_buf[kItoaBufSize];
  std::size_t line_number_width =
      itoa_to_buffer(max_line_number, max_line_buf, kItoaBufSize);

  std::vector<std::vector<Label>> label_groups;
  constexpr const std::size_t kMaxLineDistance = 3;

  for (auto&& label : sorted_labels) {
    if (!label_groups.empty()) {
      auto& current_group = label_groups.back();
      const auto& last = current_group.back();

      if (label.file_id() == last.file_id() &&
          std::abs(static_cast<int64_t>(label.range().start().line()) -
                   static_cast<int64_t>(last.range().end().line())) <=
              static_cast<int64_t>(kMaxLineDistance)) {
        current_group.push_back(std::move(label));
        continue;
      }
    }
    label_groups.emplace_back();
    label_groups.back().push_back(std::move(label));
  }

  // buffer for current line and column numbers within the loop
  char current_line_buf[kItoaBufSize];
  char current_col_buf[kItoaBufSize];
  unicode::Utf8FileId last_file_id = core::kInvalidFileId;

  for (const auto& label_group : label_groups) {
    // file id is the same but separated by group:
    // it means that the current group is far from the previous group
    if (last_file_id == label_group[0].file_id()) {
      out_str->append(line_number_width, ' ');
      out_str->append(" |   ");
      out_str->append(60, '~');
      out_str->append("\n");
      out_str->append(line_number_width, ' ');
      out_str->append(" |\n");
    } else {
      last_file_id = label_group[0].file_id();
    }

    const Label& first_label = label_group[0];
    const core::SourceLocation& loc = first_label.range().start();
    const std::size_t first_line_len =
        itoa_to_buffer(loc.line(), current_line_buf, kItoaBufSize);
    const std::size_t first_col_len =
        itoa_to_buffer(loc.column(), current_col_buf, kItoaBufSize);
    format_label_header(first_label, line_number_width, current_line_buf,
                        first_line_len, current_col_buf, first_col_len,
                        out_str);
    std::size_t current_line = loc.line();
    for (const Label& label : label_group) {
      format_label_body(label, line_number_width, current_line, out_str);
      current_line = label.range().end().line();
    }
  }
}

}  // namespace diagnostic
