// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/token/token_stream.h"

#include <string>
#include <utility>
#include <vector>

#include "core/check.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

TokenStream::TokenStream(std::vector<Token>&& tokens,
                         const core::FileManager* file_manager)
    : tokens_(std::move(tokens)),
      file_manager_(file_manager),
      current_token_(&tokens_[0]),
      end_token_(&tokens_.back()) {
  DCHECK(!tokens_.empty()) << "TokenStream requires at least one token";
}

std::string TokenStream::dump() const {
  std::string result;
  result.append("\n[token_stream]\n");
  for (const auto& token : tokens_) {
    result.append("\n[token_stream.token]\n");

    result.append("kind = ");
    result.append(token_kind_to_string(token.kind()));
    result.append("\n");

    result.append("lexeme = ");
    result.append(token.lexeme(file_manager_));
    result.append("\n");

    result.append("line = ");
    result.append(std::to_string(token.location().line()));
    result.append("\n");
    result.append("column = ");
    result.append(std::to_string(token.location().column()));
    result.append("\n");
  }
  return result;
}

}  // namespace lexer
