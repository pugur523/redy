// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <cstddef>
#include <string>

#include "core/cli/ansi/style_builder.h"
#include "frontend/diagnostic/engine/diagnostic_engine.h"
#include "i18n/base/translator.h"

namespace diagnostic {

void DiagnosticEngine::format_annotation(const Annotation& annotation,
                                         std::size_t line_number_width,
                                         std::string* out_str) const {
  const AnnotationSeverity severity = annotation.severity();
  const core::Colour severity_colour = annotation_severity_to_colour(severity);
  const char* severity_str = annotation_severity_to_string(severity);

  core::StyleBuilder s;

  out_str->append(line_number_width, ' ');
  out_str->append(" = ");

  s.style(core::Style::kBold).colour(severity_colour);
  out_str->append(s.build(severity_str));
  s.reset();

  out_str->append(": ");

  s.style(core::Style::kBold);
  if (annotation.message_tr_key() != i18n::TranslationKey::kUnknown) {
    if (annotation.should_format()) {
      char format_buf[kFormatBufSize];
      translator_->translate_fmt_to(annotation.message_tr_key(), format_buf,
                                    annotation.format_args(),
                                    annotation.args_count());
      out_str->append(s.build(format_buf));
    } else {
      out_str->append(
          s.build(translator_->translate(annotation.message_tr_key())));
    }
  }
  s.reset();

  out_str->append("\n");
}

}  // namespace diagnostic
