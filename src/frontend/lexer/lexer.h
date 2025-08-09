// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_LEXER_H_
#define FRONTEND_LEXER_LEXER_H_

#include <memory>
#include <vector>

#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/error/source_error.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/lexer/base/lexer_export.h"
#include "unicode/utf8/cursor.h"

namespace lexer {

class LEXER_EXPORT Lexer {
 public:
  using Token = base::Token;
  using TokenKind = base::TokenKind;
  using Error = diagnostic::SourceError;

  template <typename T>
  using Result = diagnostic::Result<T, Error>;
  template <typename T>
  using Results = diagnostic::Result<std::vector<T>, std::vector<Error>>;
  using InitResult =
      diagnostic::Result<std::size_t, diagnostic::DiagnosticEntry>;

  // depends on how much information to store in the output token stream.
  enum class Mode : uint8_t {
    // only essential tokens that syntactically or semantically significant
    kCodeAnalysis = 0,

    // all tokens, including all whitespace, comments, and newlines to check
    // coding style, correct format, and reconstruct the original source code.
    kFormat = 1,

    // essential tokens and all documentation comments
    // non-documentation comments and most whitespace are discarded.
    kDocumentGen = 2,
  };

  enum class Status : uint8_t {
    kNotInitialized = 0,
    kReadyToLex = 1,
    kLexCompleted = 2,
  };

  explicit Lexer(Mode mode = Mode::kCodeAnalysis);
  ~Lexer() = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) = default;
  Lexer& operator=(Lexer&&) = default;

  [[nodiscard]] InitResult init(const unicode::Utf8File& file);

  [[nodiscard]] Result<Token> next_token();

  [[nodiscard]] Results<Token> tokenize_all(bool strict = false);

 private:
  void skip_whitespace();

  Result<Token> identifier_or_keyword();
  Result<Token> literal_numeric();
  Result<Token> literal_str();
  Result<Token> literal_char();

  Result<Token> ascii_token(char current_char,
                            std::size_t start,
                            std::size_t line,
                            std::size_t col);

  Result<Token> unicode_token(char32_t current_codepoint,
                              std::size_t start,
                              std::size_t line,
                              std::size_t col);

  // operator or delimiter
  Result<Token> other_token(char current,
                            char next,
                            std::size_t start,
                            std::size_t line,
                            std::size_t col);

  inline Result<Token> create_token(TokenKind kind,
                                    std::size_t start_pos,
                                    std::size_t line,
                                    std::size_t column) {
    const std::size_t end_pos = cursor_.position();
    const std::size_t length = end_pos - start_pos;
    return Result<Token>(
        diagnostic::create_ok(Token(kind, line, column, length)));
  }

  template <typename T>
  inline Result<T> err(Error&& error) {
    return Result<T>(diagnostic::create_err(error));
  }

  unicode::Utf8Cursor cursor_;
  Mode mode_ = Mode::kCodeAnalysis;

  static constexpr const std::size_t kPredictedTokensCount = 1024;
};

}  // namespace lexer

#endif  // FRONTEND_LEXER_LEXER_H_
