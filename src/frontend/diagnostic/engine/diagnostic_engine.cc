// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/engine/diagnostic_engine.h"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "core/base/file_manager.h"
#include "core/base/logger.h"
#include "core/cli/ansi/style_builder.h"
#include "core/cli/ansi/style_util.h"
#include "frontend/diagnostic/data/annotation.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"

namespace diagnostic {

// # Samples
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

DiagnosticEngine::DiagnosticEngine(unicode::Utf8FileManager* file_manager,
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

std::string DiagnosticEngine::format_batch_and_clear() {
  Entries local;
  entries_.swap(local);

  std::string result;
  result.reserve(local.size() * kPredictedFormattedStrSize);
  format(std::move(local), &result);
  return result;
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

  core::StyleBuilder s;

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

  const core::Colour marker_colour = label_marker_type_to_colour(marker_type);
  s.style(core::Style::kBold).colour(marker_colour);
  out_str->append(s.build(src_on_marker));
  s.reset();

  out_str->append(src_after_marker);
  out_str->append("\n");

  // marker:
  // n | some code()
  //   |    **~~~~** (<- THIS PART) this code is invalid!
  out_str->append(line_number_width, ' ');
  out_str->append(" | ");
  out_str->append(marker_start_idx, ' ');

  std::string marker_string_buf;

  // heuristic
  marker_string_buf.reserve(marker_length + kFormatBufSize / 4);
  marker_string_buf.append(marker_length,
                           label_maker_type_to_char(marker_type));

  // marker message:
  // n | some code()
  //   |      ~~~~ **this code is invalid!** (<- THIS PART)
  const i18n::TranslationKey marker_msg_tr_key = label.message_tr_key();
  if (marker_msg_tr_key != i18n::TranslationKey::kDiagnosticUnknown) {
    marker_string_buf.append(" ");

    if (label.should_format()) {
      char format_buf[kFormatBufSize];
      translator_->translate_fmt_to(marker_msg_tr_key, format_buf,
                                    label.format_args(), label.args_count());
      marker_string_buf.append(format_buf);
    } else {
      marker_string_buf.append(translator_->translate(marker_msg_tr_key));
    }
  }
  s.style(core::Style::kBold).colour(marker_colour);
  out_str->append(s.build(std::move(marker_string_buf)));
  s.reset();

  out_str->append("\n");

  out_str->append(line_number_width, ' ');
  out_str->append(" |\n");
}

}  // namespace diagnostic
