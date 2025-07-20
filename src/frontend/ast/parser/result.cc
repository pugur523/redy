// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/parser/result.h"

#include <string>
#include <utility>

#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/entry_builder.h"

namespace ast {

// ParseError implementations
ParseError::ParseError(std::string_view msg,
                       const lexer::Token& token,
                       const core::FileManager* manager,
                       std::size_t position)
    : message(msg), token(&token), manager(manager), position(position) {}

std::string ParseError::format() const {
  std::string result;
  // result.reserve(512);

  if (!token || !manager) {
    result += message;
    result += "\n  at <unknown location>\n";
    return result;
  }

  const auto& loc = token->location();
  std::string_view file_name = manager->file(loc.file_id()).file_name();
  std::string_view line_view = manager->file(loc.file_id()).line(loc.line());

  result += message;
  result += "\n  --> ";
  result += file_name;
  result += ":";
  result += std::to_string(loc.line());
  result += ":";
  result += std::to_string(loc.column());
  result += "\n";
  {
    char buffer[32];

    // line
    std::size_t len = 0, n = loc.line();
    do {
      buffer[len++] = '0' + (n % 10);
      n /= 10;
    } while (n);
    for (std::size_t i = 0; i < len / 2; ++i) {
      std::swap(buffer[i], buffer[len - 1 - i]);
    }
    result.append(buffer, len);
    result += ":";

    // column
    len = 0;
    n = loc.column();
    do {
      buffer[len++] = '0' + (n % 10);
      n /= 10;
    } while (n);
    for (std::size_t i = 0; i < len / 2; ++i) {
      std::swap(buffer[i], buffer[len - 1 - i]);
    }
    result.append(buffer, len);
  }

  result += "\n   |\n";

  {
    result += " ";
    char buffer[32];
    std::size_t len = 0, n = loc.line();
    do {
      buffer[len++] = '0' + (n % 10);
      n /= 10;
    } while (n);
    for (std::size_t i = 0; i < len / 2; ++i) {
      std::swap(buffer[i], buffer[len - 1 - i]);
    }
    result.append(buffer, len);
    result += " | ";
  }

  result += line_view;
  result += "\n";

  result += "   | ";
  std::size_t start = loc.column() - 1;
  std::size_t length = token->lexeme(manager).size();
  result.append(start, ' ');
  result += '^';
  if (length > 1) {
    result.append(length - 1, '~');
  }
  result += "\n";

  return result;
}

diagnostic::DiagnosticEntry ParseError::to_entry() const {
  return diagnostic::EntryBuilder(diagnostic::Severity::kError,
                                  diagnostic::DiagnosticId::kInvalidSyntax, "")
      .label(token->location().file_id(), token->location().line(),
             token->location().column(), token->length(), "")
      .build();
}

}  // namespace ast
