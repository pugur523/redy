// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_BASE_LEX_ERROR_H_
#define FRONTEND_LEXER_BASE_LEX_ERROR_H_

#include <string>
#include <utility>

#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/lexer/base/lexer_export.h"

namespace lexer {

struct LEXER_EXPORT LexError {
  diagnostic::DiagnosticId id;
  std::size_t line;
  std::size_t column;
  std::size_t len;
  std::string message;

  inline static LexError make(diagnostic::DiagnosticId id,
                              std::size_t line,
                              std::size_t column,
                              std::size_t len,
                              std::string&& message) {
    return LexError{.id = id,
                    .line = line,
                    .column = column,
                    .len = len,
                    .message = std::move(message)};
  }
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_BASE_LEX_ERROR_H_

