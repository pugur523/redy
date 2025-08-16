// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_PARSER_PARSER_H_
#define FRONTEND_PROCESSOR_PARSER_PARSER_H_

#include <memory>
#include <utility>
#include <vector>

#include "frontend/base/token/token_kind.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/data/ast/base/nodes.h"
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

  Parser(Parser&&) = default;
  Parser& operator=(Parser&&) = default;

  void init(base::TokenStream* stream,
            unicode::Utf8FileId file_id,
            const i18n::Translator& translater);

  inline const ast::Context& context() const { return *context_; }

  Results parse_all(bool strict = false);

 private:
  Result<void> parse_root();

  // expression wo block
  Result<ast::LiteralExpressionNode> parse_literal_expression();
  Result<ast::PathExpressionNode> parse_path_expression();
  Result<ast::UnaryOperatorExpressionNode> parse_unary_operator_expression();
  Result<ast::BinaryOperatorExpressionNode> parse_binary_operator_expression();
  Result<ast::GroupedExpressionNode> parse_grouped_expression();
  Result<ast::ArrayExpressionNode> parse_array_expression();
  Result<ast::TupleExpressionNode> parse_tuple_expression();
  Result<ast::IndexExpressionNode> parse_index_expression();
  Result<ast::ConstructExpressionNode> parse_construct_expression();
  Result<ast::FunctionCallExpressionNode> parse_function_call_expression();
  Result<ast::MethodCallExpressionNode> parse_method_call_expression();
  Result<ast::MacroCallExpressionNode> parse_macro_call_expression();
  Result<ast::FieldAccessExpressionNode> parse_field_access_expression();
  Result<ast::AwaitExpressionNode> parse_await_expression();
  Result<ast::ContinueExpressionNode> parse_continue_expression();
  Result<ast::BreakExpressionNode> parse_break_expression();
  Result<ast::ExclusiveRangeExpressionNode> parse_exclusive_range_expression();
  Result<ast::InclusiveRangeExpressionNode> parse_inclusive_range_expression();
  Result<ast::ReturnExpressionNode> parse_return_expression();

  // expression w block
  Result<ast::BlockExpressionNode> parse_block_expression();
  Result<ast::UnsafeExpressionNode> parse_unsafe_expression();
  Result<ast::FastExpressionNode> parse_fast_expression();
  Result<ast::IfExpressionNode> parse_if_expression();
  Result<ast::LoopExpressionNode> parse_loop_expression();
  Result<ast::WhileExpressionNode> parse_while_expression();
  Result<ast::ForExpressionNode> parse_for_expression();
  Result<ast::MatchExpressionNode> parse_match_expression();
  Result<ast::ClosureExpressionNode> parse_closure_expression();

  // statement
  Result<void> parse_statement();
  Result<ast::AssignStatementNode> parse_assign_statement(
      ast::BuiltinAttribute attribute);
  Result<ast::AttributeStatementNode> parse_attribute_statement();
  Result<ast::ExpressionStatementNode> parse_expression_statement();

  // declaration
  Result<void> parse_declaration();
  Result<ast::FunctionDeclarationNode> parse_function_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::StructDeclarationNode> parse_struct_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::EnumerationDeclarationNode> parse_enumeration_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::TraitDeclarationNode> parse_trait_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::ImplementationDeclarationNode> parse_impl_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::UnionDeclarationNode> parse_union_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::ModuleDeclarationNode> parse_module_declaration(
      ast::BuiltinAttribute attribute);
  Result<ast::ModuleDeclarationNode> parse_redirect_declaration(
      ast::BuiltinAttribute attribute);

  // chore
  Result<ast::ParameterNode> parse_parameter_one();
  Result<ast::NodeRange> parse_parameter_list();
  Result<ast::TypeReferenceNode> parse_type_reference();

  void init_context();

  void append_errors(std::vector<De>&& new_errors);
  Result<const base::Token*> consume(base::TokenKind expected);
  void synchronize();

  inline bool eof() const { return stream_->eof(); }

  inline const base::Token& peek() const { return stream_->peek(); }
  inline const base::Token& peek_at(std::size_t offset) const {
    return stream_->peek(offset);
  }
  inline bool check(base::TokenKind kind) const { return stream_->check(kind); }

  inline const base::Token& next() { return stream_->next(); }

  // TODO: T support
  inline static Results ok() { return Results(diagnostic::create_ok()); }
  inline static Results err(std::vector<De>&& error) {
    return Results(diagnostic::create_err(std::move(error)));
  }
  template <typename T>
  inline static Result<T> ok(T&& ok_value) {
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
  unicode::Utf8FileId file_id_;
  Status status_ = Status::kNotInitialized;
};

}  // namespace parser

#endif  // FRONTEND_PROCESSOR_PARSER_PARSER_H_
