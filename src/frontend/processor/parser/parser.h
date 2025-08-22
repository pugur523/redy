// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_PARSER_PARSER_H_
#define FRONTEND_PROCESSOR_PARSER_PARSER_H_

#include <memory>
#include <utility>
#include <vector>

#include "frontend/base/operator/operator.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/data/ast/context.h"
#include "frontend/diagnostic/data/error/source_error.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/processor/parser/base/parser_export.h"
#include "unicode/utf8/file_manager.h"

namespace diagnostic {
class EntryBuilder;
class DiagnosticEntry;
}  // namespace diagnostic

namespace i18n {
class Translator;
}  // namespace i18n

namespace parser {

class PARSER_EXPORT Parser {
  using Eb = diagnostic::EntryBuilder;
  using De = diagnostic::DiagnosticEntry;

 public:
  template <typename T>
  using Result = diagnostic::Result<T, De>;
  using Results = diagnostic::Result<void, std::vector<De>>;

  enum class Status : uint8_t {
    kNotInitialized = 0,
    kReadyToParse = 1,
    kErrorOccured = 2,
    kParseCompleted = 3,
  };

  Parser() = default;
  ~Parser() = default;

  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;

  Parser(Parser&&) noexcept = default;
  Parser& operator=(Parser&&) noexcept = default;

  void init(base::TokenStream* stream, const i18n::Translator& translater);

  inline const ast::Context& context() const { return *context_; }

  Results parse_all(bool strict = false);

 private:
  using NodeId = ast::NodeId;
  using NodeRange = ast::NodeRange;
  using PayloadId = ast::PayloadId;
  using PayloadRange = ast::PayloadRange;
  using OperatorPrecedence = base::OperatorPrecedence;

  Result<NodeId> parse_root();

  // expression wo block
  Result<NodeId> parse_expression();
  Result<NodeId> parse_primary_expression();
  Result<NodeId> parse_postfix_expression();
  Result<NodeId> parse_literal_expression();
  Result<NodeId> parse_path_expression(PayloadId first_part);
  Result<NodeId> parse_unary_expression();
  Result<NodeId> parse_binary_expression(OperatorPrecedence min_precedence);
  Result<NodeId> parse_grouped_expression();
  Result<NodeId> parse_array_expression();
  Result<NodeId> parse_tuple_expression();
  Result<NodeId> parse_index_expression(NodeId operand);
  Result<NodeId> parse_construct_expression(NodeId type_path);
  Result<NodeId> parse_function_call_expression(NodeId callee);
  Result<NodeId> parse_method_call_expression(NodeId obj, PayloadId method);
  Result<NodeId> parse_function_macro_call_expression(NodeId callee);
  Result<NodeId> parse_method_macro_call_expression(NodeId obj,
                                                    PayloadId method);
  Result<NodeId> parse_field_access_expression(NodeId obj, PayloadId field);
  Result<NodeId> parse_await_expression(NodeId callee);
  Result<NodeId> parse_continue_expression();
  Result<NodeId> parse_break_expression();
  Result<NodeId> parse_range_expression();
  Result<NodeId> parse_return_expression();

  // expression w block
  Result<NodeId> parse_block_expression();
  Result<NodeId> parse_unsafe_expression();
  Result<NodeId> parse_fast_expression();
  Result<NodeId> parse_if_expression();
  Result<NodeId> parse_loop_expression();
  Result<NodeId> parse_while_expression();
  Result<NodeId> parse_for_expression();
  Result<NodeId> parse_match_expression();
  Result<NodeId> parse_closure_expression();

  // statement
  Result<NodeId> parse_statement();
  Result<NodeId> parse_assign_statement(PayloadId attribute);
  Result<NodeId> parse_attribute_statement();
  Result<NodeId> parse_expression_statement();

  // declaration
  Result<NodeId> parse_declaration();
  Result<NodeId> parse_function_declaration(PayloadId attribute);
  Result<NodeId> parse_struct_declaration(PayloadId attribute);
  Result<NodeId> parse_enumeration_declaration(PayloadId attribute);
  Result<NodeId> parse_trait_declaration(PayloadId attribute);
  Result<NodeId> parse_impl_declaration(PayloadId attribute);
  Result<NodeId> parse_union_declaration(PayloadId attribute);
  Result<NodeId> parse_module_declaration(PayloadId attribute);
  Result<NodeId> parse_redirect_declaration(PayloadId attribute);

  // chore
  Result<ast::PayloadId> parse_capture_one();
  Result<ast::PayloadRange> parse_capture_list();
  Result<ast::PayloadId> parse_parameter_one();
  Result<ast::PayloadRange> parse_parameter_list();
  Result<ast::PayloadId> parse_type_reference();
  Result<NodeRange> parse_expression_sequence();

  void init_context();

  void append_errors(std::vector<De>&& new_errors);
  Result<const base::Token*> consume(base::TokenKind expected,
                                     bool skip_whitespaces);
  void synchronize();

  inline bool eof() const { return stream_->eof(); }

  inline const base::Token& peek() const { return stream_->peek(); }
  inline const base::Token& peek_at(std::size_t offset) const {
    if (stream_->position() + offset >= stream_->size()) {
      // returns eof if out of range
      return stream_->peek(stream_->size() - stream_->position() - 1);
    }
    return stream_->peek(offset);
  }
  inline bool check(base::TokenKind kind) const { return stream_->check(kind); }
  inline const base::Token& next() { return stream_->next(); }
  inline const base::Token& next_non_whitespace() {
    base::TokenKind kind = peek().kind();
    while (!eof() && (kind == base::TokenKind::kNewline ||
                      kind == base::TokenKind::kWhitespace)) {
      kind = stream_->next().kind();
    }
    return peek();
  }

  // TODO: T support
  inline static Results ok() { return Results(diagnostic::create_ok()); }
  inline static Results err(std::vector<De>&& error) {
    return Results(diagnostic::create_err(std::move(error)));
  }
  template <typename T>
  inline static Result<T> ok(T ok_value) {
    return Result<T>(diagnostic::create_ok(std::move(ok_value)));
  }
  template <typename T>
  inline static Result<T> err(De&& err_value) {
    return Result<T>(diagnostic::create_err(std::move(err_value)));
  }
  template <typename>
  inline static Result<void> ok() {
    return Result<void>(diagnostic::create_ok());
  }

  static bool is_sync_point(base::TokenKind kind);

  std::vector<De> errors_;
  std::unique_ptr<ast::Context> context_ = nullptr;
  const i18n::Translator* translator_ = nullptr;
  base::TokenStream* stream_ = nullptr;
  Status status_ = Status::kNotInitialized;
};

}  // namespace parser

#endif  // FRONTEND_PROCESSOR_PARSER_PARSER_H_
