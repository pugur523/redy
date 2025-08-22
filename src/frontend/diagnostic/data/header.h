// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_HEADER_H_
#define FRONTEND_DIAGNOSTIC_DATA_HEADER_H_

#include <string_view>

#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "i18n/base/data/translation_impl.h"
#include "i18n/base/data/translation_key.h"

namespace diagnostic {

class DIAGNOSTIC_EXPORT Header {
 public:
  Header(Severity severity, DiagnosticId diag_id);

  ~Header() = default;

  Header(const Header&) = delete;
  Header& operator=(const Header&) = delete;

  Header(Header&&) noexcept = default;
  Header& operator=(Header&&) noexcept = default;

  inline Severity severity() const { return severity_; }
  inline DiagnosticId diag_id() const { return diag_id_; }

 private:
  Severity severity_;
  DiagnosticId diag_id_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_HEADER_H_
