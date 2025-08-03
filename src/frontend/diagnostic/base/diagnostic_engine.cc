// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/base/diagnostic_engine.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "frontend/diagnostic/base/style.h"
#include "frontend/diagnostic/data/diagnostic_id.h"

namespace diagnostic {

// ### Samples
//
// error: [undeclared_variable] use of undeclared variable `x`
//  --> src/main.ry:5:10
//   |
// 5 | y := x + 1;
//   |      ^ use of undeclared variable `x`
//   |
//   = note: `x` was not declared in this scope
//   = help: did you mean `z`?
//
// error: [move_after_borrow] value moved after borrow
//  --> src/lib.ry:12:9
//    |
// 10 | ref := &data;
//    |         ---- borrow occurs here
//    |
// 11 | println#("{}", ref);
// 12 | consume(data);
//    |         ^~~~ move occurs here
//    |
//    = note: `data` was borrowed, but then moved
//    = help: try cloning `data` if you need to use it after moving

DiagnosticEngine::DiagnosticEngine(const core::FileManager* file_manager,
                                   DiagnosticOptions options)
    : file_manager_(file_manager), options_(options) {
  DCHECK(file_manager_);
}

std::string DiagnosticEngine::pop_and_format() {
  DCHECK(!entries_.empty());
  DiagnosticEntry entry = std::move(entries_.back());
  entries_.pop_back();

  std::string result;
  result.reserve(kPredictedFormattedStrSize);
  format(std::move(entry), &result);
  return result;
}

std::string DiagnosticEngine::format_batch() {
  Entries local;
  entries_.swap(local);

  std::string result;
  result.reserve(local.size() * kPredictedFormattedStrSize);
  format(std::move(local), &result);
  return result;
}

void DiagnosticEngine::format_annotation(const Annotation& annotation,
                                         std::size_t line_number_width,
                                         std::string* out_str) const {
  out_str->append(line_number_width, ' ');
  out_str->append(" = ");
  out_str->append(style(Style::kBold));
  out_str->append(style(annotation_kind_to_style(annotation.kind())));
  out_str->append(annotation_kind_to_string(annotation.kind()));
  out_str->append(style(Style::kReset));
  out_str->append(": ");
  out_str->append(style(Style::kBold));
  out_str->append(annotation.message());
  out_str->append(style(Style::kReset));
  out_str->append("\n");
}

void DiagnosticEngine::format_label_header(const Label& label,
                                           std::size_t line_number_width,
                                           const char* line_num_str,
                                           std::size_t line_num_len,
                                           const char* col_num_str,
                                           std::size_t col_num_len,
                                           std::string* out_str) const {
  const core::File& file = file_manager_->file(label.file_id());
  const std::string& file_name = file.file_name();

  out_str->append(line_number_width, ' ');
  out_str->append("-> ");
  out_str->append(style(Style::kBold));
  out_str->append(file_name);
  out_str->append(style(Style::kReset));
  out_str->append(":");
  out_str->append(style(Style::kBrightGreen));
  out_str->append(style(Style::kBold));

  out_str->append(line_num_str, line_num_len);

  out_str->append(style(Style::kReset));
  out_str->append(":");
  out_str->append(style(Style::kBrightGreen));
  out_str->append(style(Style::kBold));

  out_str->append(col_num_str, col_num_len);

  out_str->append(style(Style::kReset));
  out_str->append("\n");

  out_str->append(line_number_width, ' ');
  out_str->append(" |\n");
}

void DiagnosticEngine::format_label(const Label& label,
                                    std::size_t line_number_width,
                                    const char* line_num_str,
                                    std::size_t line_num_len,
                                    std::string* out_str) const {
  const core::File& file = file_manager_->file(label.file_id());
  const core::SourceLocation& loc = label.range().start();
  std::string_view line = file.line(loc.line());

  // pad line number manually, append only once
  out_str->append(line_number_width - line_num_len, ' ');
  out_str->append(line_num_str, line_num_len);
  out_str->append(" | ");

  render_source_line(line, loc.column(), label.range().end().column(),
                     label.message(), line_number_width, out_str);

  for (const auto& ann : label.annotations()) {
    format_annotation(ann, line_number_width, out_str);
  }
}

void DiagnosticEngine::format(DiagnosticEntry&& entry,
                              std::string* out_str) const {
  const Header& header = entry.header();
  const Style severity_style = severity_to_style(header.severity());

  out_str->append(style(Style::kBold));
  out_str->append(style(severity_style));
  out_str->append(severity_to_string(header.severity()));
  out_str->append(style(Style::kReset));
  out_str->append(": ");

  out_str->append(style(Style::kBold));
  out_str->append(style(severity_style));
  out_str->append("[");
  char diag_code_buf[6];
  diagnostic_id_to_code(header.diag_id(), header.severity(), diag_code_buf);
  out_str->append(diag_code_buf, 5);  // except null-termination
  out_str->append("/");
  out_str->append(diagnostic_id_to_str(header.diag_id()));
  out_str->append("] ");
  out_str->append(style(Style::kReset));

  out_str->append(style(Style::kBold));
  out_str->append(header.message());
  out_str->append(style(Style::kReset));
  out_str->append("\n");

  const auto& labels = entry.labels();

  std::size_t max_line_number = 0;
  for (const Label& label : labels) {
    max_line_number = std::max(max_line_number, label.range().start().line());
  }

  char max_line_buf[16];
  std::size_t line_number_width = itoa_to_buffer(max_line_number, max_line_buf);

  // buffer for current line and column numbers within the loop
  char current_line_buf[16];
  char current_col_buf[16];

  core::FileId last_file_id = core::kInvalidFileId;
  for (const Label& label : labels) {
    const core::SourceLocation& loc = label.range().start();

    std::size_t current_line_len = itoa_to_buffer(loc.line(), current_line_buf);
    std::size_t current_col_len = itoa_to_buffer(loc.column(), current_col_buf);

    if (last_file_id != label.file_id()) {
      last_file_id = label.file_id();
      format_label_header(label, line_number_width, current_line_buf,
                          current_line_len, current_col_buf, current_col_len,
                          out_str);
    } else {
      out_str->append(line_number_width, ' ');
      out_str->append(" |   ");
      out_str->append(40, '.');
      out_str->append("\n");
      out_str->append(line_number_width, ' ');
      out_str->append(" |\n");
    }
    format_label(label, line_number_width, current_line_buf, current_line_len,
                 out_str);
  }
}

void DiagnosticEngine::format(Entries&& entries, std::string* out_str) const {
  bool first = true;
  for (DiagnosticEntry& e : entries) {
    if (!first) {
      out_str->append("\n");
    } else {
      first = false;
    }
    format(std::move(e), out_str);
  }
}

void DiagnosticEngine::render_source_line(std::string_view line,
                                          std::size_t column_start,
                                          std::size_t column_end,
                                          const std::string& marker_msg,
                                          std::size_t line_number_width,
                                          std::string* out_str) const {
  if (column_start == 0 || column_start > line.length()) {
    column_start = 1;
  }
  if (column_end <= column_start) {
    column_end = column_start + 1;
  }

  std::size_t start_idx = column_start - 1;
  std::size_t end_idx = std::min(column_end - 1, line.length());

  out_str->append(line);
  out_str->append("\n");

  out_str->append(line_number_width, ' ');
  out_str->append(" | ");
  out_str->append(start_idx, ' ');
  out_str->append(style(Style::kBold));
  out_str->append(style(Style::kBrightCyan));

  if (end_idx > start_idx + 1) {
    out_str->append("^");
    out_str->append(end_idx - start_idx - 1, '~');
  } else {
    out_str->append("^");
  }

  if (!marker_msg.empty()) {
    out_str->append(" ");
    out_str->append(marker_msg);
  }

  out_str->append(style(Style::kReset));
  out_str->append("\n");

  out_str->append(line_number_width, ' ');
  out_str->append(" |\n");
}

// static
void DiagnosticEngine::indent(std::string* out_str, std::size_t count) {
  out_str->append(count * 2, ' ');
}

// static
std::size_t DiagnosticEngine::itoa_to_buffer(int value, char* buffer) {
  // helper function to convert integer to string into a buffer
  // returns the number of characters written.

  // FIXME: this is a simplified itoa. for full robustness (negative numbers,
  // edge cases), use more comprehensive implementation or std::to_chars

  if (value == 0) {
    *buffer = '0';
    return 1;
  }
  char temp_buffer[32];  // max 10 digits for int + sign + null terminator
  char* current = temp_buffer;
  bool is_negative = false;

  if (value < 0) {
    is_negative = true;
    value = -value;  // convert to positive for conversion
  }

  while (value > 0) {
    *current++ = (value % 10) + '0';
    value /= 10;
  }

  if (is_negative) {
    *current++ = '-';
  }

  std::size_t length = current - temp_buffer;
  std::reverse(temp_buffer, current);  // reverse the digits

  // copy to the actual buffer
  for (std::size_t i = 0; i < length; ++i) {
    buffer[i] = temp_buffer[i];
  }
  return length;
}

}  // namespace diagnostic
