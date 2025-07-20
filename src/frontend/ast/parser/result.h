// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_PARSER_RESULT_H_
#define FRONTEND_AST_PARSER_RESULT_H_

#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "core/base/file_manager.h"
#include "frontend/ast/base/ast_export.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/lexer/token/token.h"

namespace ast {

// Error information
struct AST_EXPORT ParseError {
  std::string message;
  const lexer::Token* token = nullptr;
  const core::FileManager* manager = nullptr;
  std::size_t position;

  // Constructor
  ParseError(std::string_view msg,
             const lexer::Token& token,
             const core::FileManager* manager,
             std::size_t position);

  ~ParseError() = default;

  ParseError(const ParseError&) = default;
  ParseError& operator=(const ParseError&) = default;

  ParseError(ParseError&&) = default;
  ParseError& operator=(ParseError&&) = default;

  // Formats the error message for display
  std::string format() const;
  diagnostic::DiagnosticEntry to_entry() const;
};

// Result type for parser operations
template <typename T>
class AST_EXPORT Result {
 public:
  // Success constructor
  explicit Result(T value) : data_(std::move(value)) {}

  // Error constructor
  explicit Result(ParseError error) : data_(std::move(error)) {}

  // Check if result is successful
  bool is_ok() const { return std::holds_alternative<T>(data_); }
  bool is_error() const { return std::holds_alternative<ParseError>(data_); }

  // Access value (only call if is_ok() returns true)
  T& value() & { return std::get<T>(data_); }
  const T& value() const& { return std::get<T>(data_); }
  T&& value() && { return std::get<T>(std::move(data_)); }

  // Access error (only call if is_error() returns true)
  const ParseError& error() const& { return std::get<ParseError>(data_); }
  ParseError&& error() && { return std::get<ParseError>(std::move(data_)); }

  // Convenience methods
  T value_or(T default_value) const& {
    return is_ok() ? value() : std::move(default_value);
  }
  T value_or(T default_value) && {
    return is_ok() ? std::move(*this).value() : std::move(default_value);
  }

  // Transform the value if successful
  template <typename F>
  auto map(F&& func) -> Result<std::invoke_result_t<F, T>> {
    using U = std::invoke_result_t<F, T>;
    if (is_ok()) {
      return Result<U>(
          func(std::move(*this)
                   .value()));  // Use std::move for potentially rvalue func
    }
    return Result<U>(error());
  }

  // Chain operations that return Results
  template <typename F>
  auto and_then(F&& func) -> std::invoke_result_t<F, T> {
    if (is_ok()) {
      return func(std::move(*this)
                      .value());  // Use std::move for potentially rvalue func
    }
    using RetType = std::invoke_result_t<F, T>;
    return RetType(error());
  }

 private:
  std::variant<T, ParseError> data_;
};

// Multiple errors can be collected
using ParseErrors = std::vector<ParseError>;

// Result with multiple possible errors
template <typename T>
class AST_EXPORT MultiResult {
 public:
  // Success constructor
  explicit MultiResult(T value) : value_(std::move(value)) {}

  // Error constructor
  explicit MultiResult(ParseErrors&& errors) : errors_(std::move(errors)) {}

  // Single error constructor
  explicit MultiResult(const ParseError& error);

  bool is_ok() const { return value_.has_value(); }
  bool has_errors() const { return !errors_.empty(); }

  const T& value() const { return *value_; }
  T& value() { return *value_; }
  T&& take_value() { return std::move(*value_); }

  const ParseErrors& errors() const { return errors_; }
  ParseErrors& errors() { return errors_; }
  ParseErrors&& take_errors() { return std::move(errors_); }

  // Add an error
  void add_error(const ParseError& error);

  // Merge with another MultiResult
  void merge(MultiResult&& other);

 private:
  std::optional<T> value_;
  ParseErrors errors_;
};

// Helper macros for error handling
#define PARSER_TRY(expr)                                       \
  do {                                                         \
    auto result = (expr);                                      \
    if (result.is_error()) {                                   \
      return Result<decltype(result.value())>(result.error()); \
    }                                                          \
  } while (0)

#define PARSER_TRY_ASSIGN(var, expr)                                         \
  do {                                                                       \
    auto result = (expr);                                                    \
    if (result.is_error()) {                                                 \
      return Result<std::decay_t<decltype(result.value())>>(result.error()); \
    }                                                                        \
    var = std::move(result).value();                                         \
  } while (0)

// Helper functions for creating results
template <typename T>
Result<T> ok(T value) {
  return Result<T>(std::move(value));
}

template <typename T>
Result<T> error(std::string_view message,
                const lexer::Token& token,
                const core::FileManager* manager,
                std::size_t position) {
  return Result<T>(ParseError(message, token, manager, position));
}

template <typename T>
MultiResult<T> multi_ok(T value) {
  return MultiResult<T>(std::move(value));
}

template <typename T>
MultiResult<T> multi_error(ParseError error) {
  return MultiResult<T>(std::move(error));
}

template <typename T>
MultiResult<T> multi_error(ParseErrors errors) {
  return MultiResult<T>(std::move(errors));
}

// Single error constructor for MultiResult
template <typename T>
MultiResult<T>::MultiResult(const ParseError& error) {
  errors_.push_back(error);
  value_.reset();  // Ensure value is empty when initialized with an error
}

// Add an error for MultiResult
template <typename T>
void MultiResult<T>::add_error(const ParseError& error) {
  errors_.push_back(error);
  value_.reset();  // Clear value on error
}

// Merge with another MultiResult
template <typename T>
void MultiResult<T>::merge(MultiResult&& other) {
  if (other.has_errors()) {
    auto other_errors = other.take_errors();
    errors_.insert(errors_.end(), std::make_move_iterator(other_errors.begin()),
                   std::make_move_iterator(other_errors.end()));
    value_.reset();  // Clear value if merged errors exist
  }
}

}  // namespace ast

#endif  // FRONTEND_AST_PARSER_RESULT_H_
