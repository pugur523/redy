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
  std::string message;
  std::size_t line;
  std::size_t column;
  std::size_t len;
  diagnostic::DiagnosticId id;

  inline static LexError make(diagnostic::DiagnosticId id,
                              std::size_t line,
                              std::size_t column,
                              std::size_t len,
                              std::string&& message) {
    return LexError{
        .message = std::move(message),
        .line = line,
        .column = column,
        .len = len,
        .id = id,
    };
  }
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_BASE_LEX_ERROR_H_
