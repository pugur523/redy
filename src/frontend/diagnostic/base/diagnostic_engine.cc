// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/base/diagnostic_engine.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "core/base/logger.h"
#include "frontend/diagnostic/base/style.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/label.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"

namespace diagnostic {

// ### Samples
//
// error: [e0001] - undeclared variable
//  --> src/main.ry:5:10
//   |
// 5 | y := x + 1;
//   |      ~ use of undeclared variable `x`
//   |
//   = note: `x` was not declared in this scope
//   = help: did you mean `z`?
//
// error: [e0003] - move_variable_that_was_still_borrowed
//  --> src/lib.ry:12:9
//    |
// 10 | ref := &data;
//    |         ---- borrow occurs here
//    |
// 11 | println#("{}", ref);
// 12 | consume(data);
//    |         ~~~~ move occurs here
//    |
//    = note: `data` was moved while borrowed
//    = help: try cloning `data` if you need to use it after moving

DiagnosticEngine::DiagnosticEngine(const core::FileManager* file_manager,
                                   const i18n::Translator* translator,
                                   DiagnosticOptions options)
    : file_manager_(file_manager), translator_(translator), options_(options) {
  DCHECK(file_manager_);
  DCHECK(translator_);
}

std::string DiagnosticEngine::pop_and_format() {
  DCHECK(!entries_.empty());
  DiagnosticEntry entry = std::move(entries_.back());
  entries_.pop_back();

  std::string result;
  result.reserve(kPredictedFormattedStrSize);
  format_one(std::move(entry), &result);
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
  out_str->append(bold());
  out_str->append(style(annotation_severity_to_style(annotation.severity())));
  out_str->append(annotation_severity_to_string(annotation.severity()));
  out_str->append(reset());
  out_str->append(": ");
  out_str->append(bold());
  if (annotation.message_tr_key() != i18n::TranslationKey::kUnknown) {
    if (annotation.should_format()) {
      char format_buf[kFormatBufSize];
      translator_->translate_fmt_to(annotation.message_tr_key(), format_buf,
                                    annotation.format_args(),
                                    annotation.args_count());
      out_str->append(format_buf);
    } else {
      out_str->append(translator_->translate(annotation.message_tr_key()));
    }
  }
  out_str->append(reset());
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
  out_str->append(bold());
  out_str->append(file_name);
  out_str->append(reset());
  out_str->append(":");
  out_str->append(style(Style::kBrightGreen));
  out_str->append(bold());

  out_str->append(line_num_str, line_num_len);

  out_str->append(reset());
  out_str->append(":");
  out_str->append(style(Style::kBrightGreen));
  out_str->append(bold());

  out_str->append(col_num_str, col_num_len);

  out_str->append(reset());
  out_str->append("\n");

  out_str->append(line_number_width, ' ');
  out_str->append(" |\n");
}

void DiagnosticEngine::format_label_body(const Label& label,
                                         std::size_t line_number_width,
                                         std::size_t current_line,
                                         std::string* out_str) const {
  const core::File& file = file_manager_->file(label.file_id());
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
  core::FileId last_file_id = core::kInvalidFileId;

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

void DiagnosticEngine::format_header(const Header& header,
                                     std::string* out_str) const {
  const Style severity_style = severity_to_style(header.severity());
  out_str->append(bold());
  out_str->append(style(severity_style));
  out_str->append(
      translator_->translate(severity_to_tr_key(header.severity())));
  out_str->append(reset());
  out_str->append(": ");

  out_str->append(bold());
  out_str->append(style(severity_style));
  out_str->append("[");
  char diag_code_buf[6];
  diagnostic_id_to_code(header.diag_id(), header.severity(), diag_code_buf);
  out_str->append(diag_code_buf, 5);  // except null-termination
  out_str->append("]");
  out_str->append(reset());

  out_str->append(" - ");

  out_str->append(bold());
  out_str->append(
      translator_->translate(diagnostic_id_to_tr_key(header.diag_id())));
  out_str->append(reset());
  out_str->append("\n");
}

void DiagnosticEngine::format_one(DiagnosticEntry&& entry,
                                  std::string* out_str) const {
  format_header(entry.header(), out_str);

  format_labels(std::move(entry.sort_labels()), out_str);
}

void DiagnosticEngine::format(Entries&& entries, std::string* out_str) const {
  bool first = true;
  for (DiagnosticEntry& e : entries) {
    if (!first) {
      out_str->append("\n");
    } else {
      first = false;
    }
    format_one(std::move(e), out_str);
  }
}

void DiagnosticEngine::render_source_line(std::string* out_str,
                                          const Label& label,
                                          std::string_view line,
                                          std::size_t line_number_width) const {
  // at first, the last line of the `out_str` is like:
  // "n |" (n is the line number of the source line)

  std::size_t column_start = label.range().start().column();
  std::size_t column_end = label.range().end().column();

  if (column_start == 0 || column_start > line.length()) {
    column_start = 1;
  }
  if (column_end <= column_start) {
    column_end = column_start + 1;
  }

  const std::size_t marker_start_idx = column_start - 1;
  const std::size_t marker_end_idx = std::min(column_end - 1, line.length());
  const std::size_t marker_length = marker_end_idx - marker_start_idx;
  const LabelMarkerType marker_type = label.marker_type();

  // source line:
  // n | **some code()** (<- THIS PART)
  //   |        ~~~~ this code is invalid!
  const std::string_view src_before_marker = line.substr(0, marker_start_idx);
  const std::string_view src_on_marker =
      line.substr(marker_start_idx, marker_length);
  const std::string_view src_after_marker = line.substr(marker_end_idx);

  out_str->append(src_before_marker);

  out_str->append(bold());
  out_str->append(style(label_marker_type_to_style(marker_type)));
  out_str->append(src_on_marker);
  out_str->append(reset());

  out_str->append(src_after_marker);
  out_str->append("\n");

  // marker:
  // n | some code()
  //   |    **~~~~** (<- THIS PART) this code is invalid!
  out_str->append(line_number_width, ' ');
  out_str->append(" | ");
  out_str->append(marker_start_idx, ' ');
  out_str->append(bold());
  out_str->append(style(label_marker_type_to_style(marker_type)));
  out_str->append(marker_length, label_maker_type_to_char(marker_type));

  // marker message:
  // n | some code()
  //   |      ~~~~ **this code is invalid!** (<- THIS PART)
  const i18n::TranslationKey marker_msg_tr_key = label.message_tr_key();
  if (marker_msg_tr_key != i18n::TranslationKey::kDiagnosticUnknown) {
    out_str->append(" ");

    if (label.should_format()) {
      char format_buf[kFormatBufSize];
      translator_->translate_fmt_to(marker_msg_tr_key, format_buf,
                                    label.format_args(), label.args_count());
      out_str->append(format_buf);
    } else {
      out_str->append(translator_->translate(marker_msg_tr_key));
    }
  }

  out_str->append(reset());
  out_str->append("\n");

  out_str->append(line_number_width, ' ');
  out_str->append(" |\n");
}

// static
void DiagnosticEngine::indent(std::string* out_str, std::size_t count) {
  out_str->append(count * 2, ' ');
}

// static
template <typename T>
std::size_t DiagnosticEngine::itoa_to_buffer(T value,
                                             char* buffer,
                                             std::size_t buf_size) {
  static_assert(std::is_integral_v<T>, "itoa_to_buffer only supports integers");
  auto [ptr, ec] = std::to_chars(buffer, buffer + buf_size, value);
  if (ec != std::errc{}) {
    return 0;
  }
  return static_cast<std::size_t>(ptr - buffer);
}

}  // namespace diagnostic
