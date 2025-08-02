// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/token/token.h"

#include <string>
#include <utility>

namespace base {

Token::Token(TokenKind kind,
             const core::SourceLocation& location,
             std::size_t length)
    : location_(location), length_(length), kind_(kind) {}

Token::Token(TokenKind kind,
             core::FileId file_id,
             std::size_t line,
             std::size_t column,
             std::size_t length)
    : Token(kind, core::SourceLocation(line, column, file_id), length) {}

}  // namespace base
