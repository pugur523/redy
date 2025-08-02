// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PARSER_PARSE_ERROR_H_
#define FRONTEND_PARSER_PARSE_ERROR_H_

#include <string>
#include <utility>

#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/parser/base/parser_export.h"

namespace parser {

struct PARSER_EXPORT ParseError {
  std::string message;
  std::size_t line;
  std::size_t column;
  std::size_t len;
  diagnostic::DiagnosticId id;

  inline static ParseError make(diagnostic::DiagnosticId id,
                                std::size_t line,
                                std::size_t column,
                                std::size_t len,
                                std::string&& message) {
    return ParseError{
        .message = std::move(message),
        .line = line,
        .column = column,
        .len = len,
        .id = id,
    };
  }

  inline static ParseError make(diagnostic::DiagnosticId id,
                                const base::Token& token,
                                std::string&& message) {
    return make(id, token.location().line(), token.location().column(),
                token.length(), std::move(message));
  }
};

}  // namespace parser

#endif  // FRONTEND_PARSER_PARSE_ERROR_H_
