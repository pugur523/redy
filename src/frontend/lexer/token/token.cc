// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/token/token.h"

#include <string>
#include <utility>

namespace lexer {

Token::Token(TokenKind kind,
             const core::FileManager* file_manager,
             core::SourceLocation&& location,
             std::size_t length)
    : location_(std::move(location)),
      length_(length),
      file_manager_(file_manager),
      kind_(kind) {}

Token::Token(TokenKind kind,
             const core::FileManager* file_manager,
             core::FileId file_id,
             std::size_t line,
             std::size_t column,
             std::size_t length)
    : Token(kind,
            file_manager,
            core::SourceLocation(line, column, file_id),
            length) {}

}  // namespace lexer
