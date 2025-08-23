// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_LEXER_LEXER_H_
#define FRONTEND_PROCESSOR_LEXER_LEXER_H_

#include <memory>
#include <vector>

#include "frontend/base/token/token.h"
#include "frontend/diagnostic/data/error/source_error.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/processor/lexer/base/lexer_export.h"
#include "unicode/utf8/stream.h"

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
  using InitResult = diagnostic::Result<void, diagnostic::DiagnosticEntry>;

  // depends on how much information to store in the output token stream.
  enum class Mode : uint8_t {
    // only essential tokens including newline that syntactically or
    // semantically significant
    // newlines are significant to interpret non-semicolon codes
    kCodeAnalysis = 0,

    // essential tokens and all documentation comments
    // non-documentation comments and most whitespaces are discarded.
    kDocumentGen = 1,

    // all tokens, including all whitespaces and comments to check
    // coding style, correct format, and reconstruct the original source code.
    kFormat = 2,
  };

  enum class Status : uint8_t {
    kNotInitialized = 0,
    kReadyToTokenize = 1,
    kErrorOccured = 2,
    kTokenizeCompleted = 3,
  };

  Lexer();
  ~Lexer() = default;

  Lexer(const Lexer&) = delete;
  Lexer& operator=(const Lexer&) = delete;

  Lexer(Lexer&&) noexcept = default;
  Lexer& operator=(Lexer&&) noexcept = default;

  [[nodiscard]] InitResult init(unicode::Utf8FileManager* file_manager,
                                unicode::Utf8FileId file_id,
                                Mode mode = Mode::kCodeAnalysis);

  [[nodiscard]] Results<Token> tokenize(bool strict = false);

  [[nodiscard]] Result<Token> tokenize_next();

  inline const unicode::Utf8Stream& stream() const { return stream_; }

  inline void reset() {
    DCHECK_NE(status_, Status::kNotInitialized);
    stream_.reset();
    status_ = Status::kReadyToTokenize;
  }

 private:
  void skip_whitespace();
  Result<void> skip_comments();
  Result<void> skip_trivia();

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
    const std::size_t end_pos = stream_.position();
    const std::size_t length = end_pos - start_pos;
    return Result<Token>(
        diagnostic::create_ok(Token(kind, line, column, length)));
  }

  template <typename T>
  inline Result<T> err(Error&& error) {
    status_ = Status::kErrorOccured;
    return Result<T>(diagnostic::create_err(error));
  }

  inline bool should_include_whitespace() const {
    return mode_ == Mode::kFormat;
  }

  inline bool should_include_normal_comments() const {
    return mode_ == Mode::kFormat;
  }

  inline bool should_include_documentation_comments() const {
    return mode_ == Mode::kDocumentGen;
  }

  unicode::Utf8Stream stream_;
  Mode mode_ = Mode::kCodeAnalysis;
  Status status_ = Status::kNotInitialized;

  // heuristic
  static constexpr const std::size_t kPredictedTokensCountPerLine = 12;
};

}  // namespace lexer

#endif  // FRONTEND_PROCESSOR_LEXER_LEXER_H_
