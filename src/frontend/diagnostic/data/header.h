// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_HEADER_H_
#define FRONTEND_DIAGNOSTIC_DATA_HEADER_H_

#include <string>

#include "core/base/file_manager.h"
#include "core/base/source_location.h"
#include "core/base/source_range.h"
#include "frontend/diagnostic/base/diagnostic_export.h"
#include "frontend/diagnostic/data/annotation.h"
#include "frontend/diagnostic/data/diagnostic_id.h"

namespace diagnostic {

class DIAGNOSTIC_EXPORT Header {
 public:
  Header(Severity severity, DiagnosticId diag_id, std::string&& message);

  ~Header() = default;

  Header(const Header&) = delete;
  Header& operator=(const Header&) = delete;

  Header(Header&&) = default;
  Header& operator=(Header&&) = default;

  inline const std::string& message() const { return message_; }
  inline Severity severity() const { return severity_; }
  inline DiagnosticId diag_id() const { return diag_id_; }

 private:
  std::string message_;
  Severity severity_;
  DiagnosticId diag_id_;
};

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_HEADER_H_
