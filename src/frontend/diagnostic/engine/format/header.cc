// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <cstddef>
#include <string>

#include "core/base/style_builder.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/diagnostic/engine/diagnostic_engine.h"
#include "i18n/base/translator.h"

namespace diagnostic {

void DiagnosticEngine::format_header(const Header& header,
                                     std::string* out_str) const {
  const core::Colour severity_colour = severity_to_colour(header.severity());

  core::StyleBuilder s;

  s.style(core::Style::kBold).colour(severity_colour);
  out_str->append(
      s.build(translator_->translate(severity_to_tr_key(header.severity()))));
  s.reset();

  out_str->append(": ");

  // diag_code be like "[e0001]" <- fixed length
  constexpr std::size_t kDiagCodeBufSize = 8;
  char diag_code_buf[kDiagCodeBufSize];
  diag_code_buf[0] = '[';
  diagnostic_id_to_code(header.diag_id(), header.severity(), diag_code_buf + 1);
  diag_code_buf[kDiagCodeBufSize - 1] = ']';

  s.style(core::Style::kBold).colour(severity_colour);
  out_str->append(s.build(diag_code_buf, 7));
  s.reset();

  out_str->append(" - ");

  s.style(core::Style::kBold);
  out_str->append(s.build(
      translator_->translate(diagnostic_id_to_tr_key(header.diag_id()))));
  s.reset();

  out_str->append("\n");
}

}  // namespace diagnostic
