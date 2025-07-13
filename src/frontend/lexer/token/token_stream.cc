// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/token/token_stream.h"

#include <utility>
#include <vector>

#include "core/check.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

TokenStream::TokenStream(std::vector<Token>&& tokens)
    : tokens_(std::move(tokens)),
      current_token_(&tokens_[0]),
      end_token_(&tokens_.back()) {
  DCHECK(!tokens_.empty()) << "TokenStream requires at least one token";
}

}  // namespace lexer
