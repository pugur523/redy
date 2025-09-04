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
#include "frontend/data/ast/context.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/data/ast/payload/statement.h"
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

class Parser {
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

  PARSER_EXPORT Parser() = default;
  PARSER_EXPORT ~Parser() = default;

  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;

  Parser(Parser&&) noexcept = default;
  Parser& operator=(Parser&&) noexcept = default;

  PARSER_EXPORT void init(base::TokenStream* stream,
                          const i18n::Translator& translater);

  PARSER_EXPORT Results parse_all(bool strict = false);

  inline const ast::Context& context() const { return *context_; }

 private:
  using Node = ast::Node;
  using NodeId = ast::NodeId;
  using NodeRange = ast::NodeRange;

  template <typename T>
  using PayloadId = ast::PayloadId<T>;

  template <typename T>
  using PayloadRange = ast::PayloadRange<T>;

  void parse_next();

  // temporarily disable line length limit for readability
  // clang-format off
  // NOLINTBEGIN

  // expression wo block
  Result<NodeId> parse_expression();
  Result<NodeId> parse_primary_expr();
  Result<NodeId> parse_postfix_expr();
  Result<PayloadId<ast::LiteralExpressionPayload>> parse_literal_expr();
  Result<PayloadId<ast::PathExpressionPayload>> parse_path_expr();
  Result<NodeId> parse_unary_expr();
  Result<NodeId> parse_binary_expr(base::OperatorPrecedence min_precedence);
  Result<PayloadId<ast::GroupedExpressionPayload>> parse_grouped_expr();
  Result<PayloadId<ast::ArrayExpressionPayload>> parse_array_expr();
  Result<PayloadId<ast::TupleExpressionPayload>> parse_tuple_expr();
  Result<PayloadId<ast::IndexExpressionPayload>> parse_index_expr(NodeId operand);
  Result<PayloadId<ast::ConstructExpressionPayload>> parse_construct_expr(NodeId type_path);

  // can await
  Result<NodeId> parse_function_call_expr(NodeId callee);
  // can await
  Result<NodeId> parse_method_call_expr(
      NodeId obj,
      PayloadId<ast::PathExpressionPayload> method);

  Result<PayloadId<ast::FunctionMacroCallExpressionPayload>> parse_function_macro_call_expr(NodeId callee);
  Result<PayloadId<ast::MethodMacroCallExpressionPayload>> parse_method_macro_call_expr(
      NodeId obj,
      PayloadId<ast::PathExpressionPayload> method);
  Result<PayloadId<ast::FieldAccessExpressionPayload>> parse_field_access_expr(
      NodeId obj,
      PayloadId<ast::PathExpressionPayload> field);
  Result<NodeId> parse_await_expr(NodeId callee);
  Result<PayloadId<ast::ContinueExpressionPayload>> parse_continue_expr();
  Result<PayloadId<ast::BreakExpressionPayload>> parse_break_expr();
  Result<PayloadId<ast::RangeExpressionPayload>> parse_range_expr();
  Result<PayloadId<ast::ReturnExpressionPayload>> parse_return_expr();

  // expression w block
  Result<PayloadId<ast::BlockExpressionPayload>> parse_block_expr();
  Result<PayloadId<ast::UnsafeExpressionPayload>> parse_unsafe_expr();
  Result<PayloadId<ast::FastExpressionPayload>> parse_fast_expr();
  Result<PayloadId<ast::IfExpressionPayload>> parse_if_expr();
  Result<PayloadId<ast::LoopExpressionPayload>> parse_loop_expr();
  Result<PayloadId<ast::WhileExpressionPayload>> parse_while_expr();
  Result<PayloadId<ast::ForExpressionPayload>> parse_for_expr();
  Result<PayloadId<ast::MatchExpressionPayload>> parse_match_expr();
  Result<PayloadId<ast::ClosureExpressionPayload>> parse_closure_expr();

  // statement
  using Sad = ast::StorageAttributeData;
  Result<NodeId> parse_statement();
  Result<PayloadId<ast::AssignStatementPayload>> parse_assign_stmt(Sad storage_attribute);
  Result<PayloadId<ast::AttributeStatementPayload>> parse_attribute_stmt();
  Result<PayloadId<ast::ExpressionStatementPayload>> parse_expression_stmt();

  // TODO: add template parsing for declarations, calls, and type refs
  // syntax example: some_symbol<T, E, Size: usize, UseColor: bool>

  // declaration
  Result<NodeId> parse_decl_stmt();
  Result<PayloadId<ast::FunctionDeclarationPayload>> parse_function_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::StructDeclarationPayload>> parse_struct_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::EnumerationDeclarationPayload>> parse_enumeration_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::TraitDeclarationPayload>> parse_trait_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::ImplementationDeclarationPayload>> parse_impl_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::UnionDeclarationPayload>> parse_union_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::ModuleDeclarationPayload>> parse_module_decl_stmt(Sad storage_attribute);
  Result<PayloadId<ast::RedirectDeclarationPayload>> parse_redirect_decl_stmt(Sad storage_attribute);

  // chore
  Result<PayloadId<ast::CapturePayload>> parse_capture_one();
  Result<PayloadRange<ast::CapturePayload>> parse_capture_list();
  Result<PayloadId<ast::FieldPayload>> parse_field_one();
  Result<PayloadRange<ast::FieldPayload>> parse_field_list();
  Result<PayloadId<ast::ParameterPayload>> parse_parameter_one();
  Result<PayloadRange<ast::ParameterPayload>> parse_parameter_list();
  Result<PayloadId<ast::AttributeUsePayload>> parse_attribute_use_one();
  Result<PayloadRange<ast::AttributeUsePayload>> parse_attribute_use_list();
  Result<PayloadId<ast::EnumVariantPayload>> parse_enum_variant();
  Result<PayloadId<ast::TypeReferencePayload>> parse_type_reference();

  Result<NodeRange> parse_expression_sequence();

  // NOLINTEND
  // clang-format on

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

  // void specialization
  template <typename>
  inline static Result<void> ok() {
    return Result<void>(diagnostic::create_ok());
  }

  // error result casting
  template <typename T, typename U>
  inline static Result<T> err(Result<U>&& result) {
    return Result<T>(diagnostic::create_err(std::move(result).unwrap_err()));
  }

  template <typename T>
  inline Result<NodeId> wrap_to_node(ast::NodeKind kind, Result<T>&& result) {
    if (result.is_err()) {
      return err<NodeId>(std::move(result));
    }
    return ok<NodeId>(context_->alloc(Node{
        .payload_id = std::move(result).unwrap().id,
        .kind = kind,
    }));
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
