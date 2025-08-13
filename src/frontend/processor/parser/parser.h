// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_PARSER_PARSER_H__H_
#define FRONTEND_PROCESSOR_PARSER_PARSER_H__H_

#include "frontend/base/token/token_stream.h"
#include "frontend/data/ast/context.h"
#include "frontend/diagnostic/data/error/source_error.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/processor/parser/base/parser_export.h"
namespace parser {

class PARSER_EXPORT Parser {
 public:
  using Error = diagnostic::SourceError;

  template <typename T>
  using Result = diagnostic::Result<T, Error>;
  template <typename T>
  using Results = diagnostic::Result<std::vector<T>, std::vector<Error>>;

  enum class Status : uint8_t {
    kNotInitialized = 0,
    kReadyToParse = 1,
    kErrorOccuredWhileParsing = 2,
    kParseCompleted = 3,
  };

  Parser() = default;
  ~Parser() = default;

  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;

  Parser(Parser&&) = default;
  Parser& operator=(Parser&&) = default;

  void init(const base::TokenStream& stream);

  Result<void> parse_all();

  Result<void> parse_next();

  Results<void> parse();

  inline const ast::Context& context() const { return *context_; }

 private:
  std::unique_ptr<ast::Context> context_ = nullptr;
  const base::TokenStream* stream_ = nullptr;
  Status status_ = Status::kNotInitialized;
};

}  // namespace parser

#endif  // FRONTEND_PROCESSOR_PARSER_PARSER_H__H_
