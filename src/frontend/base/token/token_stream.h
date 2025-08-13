// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_TOKEN_TOKEN_STREAM_H_
#define FRONTEND_BASE_TOKEN_TOKEN_STREAM_H_

#include <limits>
#include <string>
#include <vector>

#include "core/check.h"
#include "frontend/base/base_export.h"
#include "frontend/base/token/token.h"
#include "frontend/base/token/token_kind.h"
#include "unicode/utf8/file.h"

namespace base {

class BASE_EXPORT TokenStream {
 public:
  explicit TokenStream(std::vector<Token>&& tokens,
                       const unicode::Utf8File& file);

  ~TokenStream() = default;

  TokenStream() = delete;

  TokenStream(const TokenStream&) = delete;
  TokenStream& operator=(const TokenStream&) = delete;

  TokenStream(TokenStream&&) = default;
  TokenStream& operator=(TokenStream&&) = default;

  inline const Token& peek(std::size_t offset = 0) const;
  inline const Token& previous() const;
  inline const Token& next();
  inline bool match(TokenKind expected_kind);
  inline constexpr void rewind(std::size_t pos);
  inline constexpr bool check(TokenKind expected_kind) const;
  inline constexpr bool check(TokenKind expected_kind,
                              std::size_t offset) const;
  inline constexpr bool eof() const;
  inline constexpr std::size_t position() const;
  inline constexpr std::size_t size() const;
  inline const unicode::Utf8File& file() const;

  std::string dump() const;

 private:
  std::vector<Token> tokens_;
  const unicode::Utf8File* file_ = nullptr;
  const Token* current_token_ = nullptr;
  const Token* end_token_ = nullptr;
  std::size_t pos_ = 0;
};

inline const Token& TokenStream::peek(std::size_t offset) const {
  const std::size_t target_pos = pos_ + offset;
  DCHECK_LT(target_pos, tokens_.size());
  return tokens_[target_pos];
}

inline const Token& TokenStream::previous() const {
  DCHECK_GT(pos_, 0) << "previous token not found";
  return tokens_[pos_ - 1];
}

inline const Token& TokenStream::next() {
  DCHECK_NE(current_token_, end_token_) << "reached eof token unexpectedly";
  pos_++;
  current_token_ = &tokens_[pos_];
  return tokens_[pos_];
}

inline bool TokenStream::match(TokenKind expected_kind) {
  DCHECK_NE(current_token_, end_token_) << "reached eof token unexpectedly";
  if (peek().kind() == expected_kind) [[likely]] {
    next();
    return true;
  }
  return false;
}

inline constexpr void TokenStream::rewind(std::size_t pos) {
  DCHECK_LE(pos, tokens_.size()) << "rewind range is invalid";
  pos_ = pos;
  current_token_ = &tokens_[pos_];
}

inline constexpr bool TokenStream::check(TokenKind expected_kind) const {
  return current_token_->kind() == expected_kind;
}

inline constexpr bool TokenStream::check(TokenKind expected_kind,
                                         std::size_t offset) const {
  DCHECK_LT(pos_ + offset, size());
  return peek(offset).kind() == expected_kind;
}

inline constexpr bool TokenStream::eof() const {
  return current_token_->kind() == TokenKind::kEof;
}

inline constexpr std::size_t TokenStream::position() const {
  return pos_;
}

inline constexpr std::size_t TokenStream::size() const {
  return tokens_.size();
}

inline const unicode::Utf8File& TokenStream::file() const {
  return *file_;
}

}  // namespace base

#endif  // FRONTEND_BASE_TOKEN_TOKEN_STREAM_H_
