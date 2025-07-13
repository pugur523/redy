// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_TOKEN_TOKEN_STREAM_H_
#define FRONTEND_LEXER_TOKEN_TOKEN_STREAM_H_

#include <vector>

#include "core/check.h"
#include "frontend/lexer/base/lexer_export.h"
#include "frontend/lexer/token/token.h"

namespace lexer {

class LEXER_EXPORT TokenStream {
 public:
  explicit TokenStream(std::vector<Token>&& tokens);

  ~TokenStream() = default;

  TokenStream(const TokenStream&) = delete;
  TokenStream& operator=(const TokenStream&) = delete;

  TokenStream(TokenStream&&) noexcept = default;
  TokenStream& operator=(TokenStream&&) noexcept = default;

  inline const Token& peek(std::size_t offset = 0) const;
  inline const Token& advance();
  inline bool match(TokenKind expected_kind);

  inline constexpr void rewind(std::size_t pos) {
    DCHECK_LE(pos, tokens_.size()) << "rewind range is invalid";
    pos_ = pos;
  }

  inline constexpr bool check(TokenKind expected_kind) const {
    return current_token_->kind() == expected_kind;
  }

  inline constexpr bool eof() const {
    return current_token_->kind() == TokenKind::kEof;
  }

  inline constexpr std::size_t position() const { return pos_; }

  inline constexpr std::size_t size() const { return tokens_.size(); }

 private:
  std::vector<Token> tokens_;
  std::size_t pos_ = 0;
  const Token* current_token_ = nullptr;
  const Token* end_token_ = nullptr;
};

inline const Token& TokenStream::peek(std::size_t offset) const {
  const std::size_t target_pos = pos_ + offset;
  DCHECK_LT(target_pos, tokens_.size());
  return tokens_[target_pos];
}

inline const Token& TokenStream::advance() {
  DCHECK_NE(current_token_, end_token_) << "Reached eof token unexpectedly";
  ++pos_;
  current_token_ = &tokens_[pos_];
  return *current_token_;
}

inline bool TokenStream::match(TokenKind expected_kind) {
  if (peek().kind() == expected_kind) [[likely]] {
    advance();
    return true;
  }
  return false;
}

}  // namespace lexer

#endif  // FRONTEND_LEXER_TOKEN_TOKEN_STREAM_H_
