// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_CONTEXT_H_
#define FRONTEND_DATA_AST_CONTEXT_H_

#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

#include "frontend/base/data/arena.h"
#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"

namespace ast {

class AST_EXPORT Context {
 public:
  ~Context() = default;

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&&) = default;
  Context& operator=(Context&&) = default;

  static inline std::unique_ptr<Context> create() {
    return std::unique_ptr<Context>(new Context());
  }

  template <typename T>
  inline base::Arena<T>& arena();

  template <typename T>
  inline std::size_t alloc(T&& value) {
    return arena<std::decay_t<T>>().alloc(std::move(value));
  }

  template <typename T>
  inline std::size_t create(NodeKind kind, T&& payload) {
    const PayloadId payload_id = alloc<T>(std::move(payload));
    return alloc(Node{.kind = kind, .payload_id = payload_id});
  }

  template <typename T>
  inline T& get(std::size_t id) {
    return arena<T>()[id];
  }

 private:
  Context() = default;

  base::Arena<Node> nodes_;

  base::Arena<LiteralExpressionPayload> literal_expression_payloads_;
  base::Arena<PathExpressionPayload> path_expression_payloads_;
  base::Arena<UnaryExpressionPayload> unary_expression_payloads_;
  base::Arena<BinaryExpressionPayload> binary_expression_payloads_;
  base::Arena<GroupedExpressionPayload> grouped_expression_payloads_;
  base::Arena<ArrayExpressionPayload> array_expression_payloads_;
  base::Arena<TupleExpressionPayload> tuple_expression_payloads_;
  base::Arena<IndexExpressionPayload> index_expression_payloads_;
  base::Arena<ConstructExpressionPayload> construct_expression_payloads_;
  base::Arena<FunctionCallExpressionPayload> function_call_expression_payloads_;
  base::Arena<MethodCallExpressionPayload> method_call_expression_payloads_;
  base::Arena<FunctionMacroCallExpressionPayload> fn_macro_call_expr_payloads_;
  base::Arena<MethodMacroCallExpressionPayload> mt_macro_call_expr_payloads_;
  base::Arena<FieldAccessExpressionPayload> field_access_expression_payloads_;
  base::Arena<AwaitExpressionPayload> await_expression_payloads_;
  base::Arena<ContinueExpressionPayload> continue_expression_payloads_;
  base::Arena<BreakExpressionPayload> break_expression_payloads_;
  base::Arena<ExclusiveRangeExpressionPayload> ex_range_expression_payloads_;
  base::Arena<InclusiveRangeExpressionPayload> in_range_expression_payloads_;
  base::Arena<ReturnExpressionPayload> return_expression_payloads_;

  base::Arena<BlockExpressionPayload> block_expression_payloads_;
  base::Arena<UnsafeExpressionPayload> unsafe_expression_payloads_;
  base::Arena<FastExpressionPayload> fast_expression_payloads_;
  base::Arena<IfExpressionPayload> if_expression_payloads_;
  base::Arena<LoopExpressionPayload> loop_expression_payloads_;
  base::Arena<WhileExpressionPayload> while_expression_payloads_;
  base::Arena<ForExpressionPayload> for_expression_payloads_;
  base::Arena<MatchExpressionPayload> match_expression_payloads_;
  base::Arena<ClosureExpressionPayload> closure_expression_payloads_;

  base::Arena<AssignStatementPayload> assign_statement_payloads_;
  base::Arena<AttributeStatementPayload> attribute_statement_payloads_;
  base::Arena<RedirectStatementPayload> redirect_statement_payloads_;

  base::Arena<FunctionDeclarationPayload> function_declaration_payloads_;
  base::Arena<StructDeclarationPayload> struct_declaration_payloads_;
  base::Arena<EnumerationDeclarationPayload> enumeration_declaration_payloads_;
  base::Arena<TraitDeclarationPayload> trait_declaration_payloads_;
  base::Arena<ImplementationDeclarationPayload> impl_declaration_payloads_;
  base::Arena<UnionDeclarationPayload> union_declaration_payloads_;
  base::Arena<ModuleDeclarationPayload> module_declaration_payloads_;

  base::Arena<CapturePayload> capture_payloads_;
  base::Arena<ParameterPayload> parameter_payloads_;
  base::Arena<TypeReferencePayload> type_reference_payloads_;
  base::Arena<ArrayTypePayload> array_type_payloads_;
  base::Arena<IdentifierPayload> identifier_payloads_;
  base::Arena<StorageAttributePayload> storage_attribute_payloads_;
  base::Arena<IfBranchPayload> if_branch_payloads_;
  base::Arena<MatchArmPayload> match_arm_payloads_;
  base::Arena<VariablePayload> variable_payloads_;
};

template <>
inline base::Arena<Node>& Context::arena<Node>() {
  return nodes_;
}

template <>
inline base::Arena<LiteralExpressionPayload>&
Context::arena<LiteralExpressionPayload>() {
  return literal_expression_payloads_;
}
template <>
inline base::Arena<PathExpressionPayload>&
Context::arena<PathExpressionPayload>() {
  return path_expression_payloads_;
}
template <>
inline base::Arena<UnaryExpressionPayload>&
Context::arena<UnaryExpressionPayload>() {
  return unary_expression_payloads_;
}
template <>
inline base::Arena<BinaryExpressionPayload>&
Context::arena<BinaryExpressionPayload>() {
  return binary_expression_payloads_;
}
template <>
inline base::Arena<GroupedExpressionPayload>&
Context::arena<GroupedExpressionPayload>() {
  return grouped_expression_payloads_;
}
template <>
inline base::Arena<ArrayExpressionPayload>&
Context::arena<ArrayExpressionPayload>() {
  return array_expression_payloads_;
}
template <>
inline base::Arena<TupleExpressionPayload>&
Context::arena<TupleExpressionPayload>() {
  return tuple_expression_payloads_;
}
template <>
inline base::Arena<IndexExpressionPayload>&
Context::arena<IndexExpressionPayload>() {
  return index_expression_payloads_;
}
template <>
inline base::Arena<ConstructExpressionPayload>&
Context::arena<ConstructExpressionPayload>() {
  return construct_expression_payloads_;
}
template <>
inline base::Arena<FunctionCallExpressionPayload>&
Context::arena<FunctionCallExpressionPayload>() {
  return function_call_expression_payloads_;
}
template <>
inline base::Arena<MethodCallExpressionPayload>&
Context::arena<MethodCallExpressionPayload>() {
  return method_call_expression_payloads_;
}
template <>
inline base::Arena<FunctionMacroCallExpressionPayload>&
Context::arena<FunctionMacroCallExpressionPayload>() {
  return fn_macro_call_expr_payloads_;
}
template <>
inline base::Arena<MethodMacroCallExpressionPayload>&
Context::arena<MethodMacroCallExpressionPayload>() {
  return mt_macro_call_expr_payloads_;
}
template <>
inline base::Arena<FieldAccessExpressionPayload>&
Context::arena<FieldAccessExpressionPayload>() {
  return field_access_expression_payloads_;
}
template <>
inline base::Arena<AwaitExpressionPayload>&
Context::arena<AwaitExpressionPayload>() {
  return await_expression_payloads_;
}
template <>
inline base::Arena<ContinueExpressionPayload>&
Context::arena<ContinueExpressionPayload>() {
  return continue_expression_payloads_;
}
template <>
inline base::Arena<BreakExpressionPayload>&
Context::arena<BreakExpressionPayload>() {
  return break_expression_payloads_;
}
template <>
inline base::Arena<ExclusiveRangeExpressionPayload>&
Context::arena<ExclusiveRangeExpressionPayload>() {
  return ex_range_expression_payloads_;
}
template <>
inline base::Arena<InclusiveRangeExpressionPayload>&
Context::arena<InclusiveRangeExpressionPayload>() {
  return in_range_expression_payloads_;
}
template <>
inline base::Arena<ReturnExpressionPayload>&
Context::arena<ReturnExpressionPayload>() {
  return return_expression_payloads_;
}
template <>
inline base::Arena<BlockExpressionPayload>&
Context::arena<BlockExpressionPayload>() {
  return block_expression_payloads_;
}
template <>
inline base::Arena<UnsafeExpressionPayload>&
Context::arena<UnsafeExpressionPayload>() {
  return unsafe_expression_payloads_;
}
template <>
inline base::Arena<FastExpressionPayload>&
Context::arena<FastExpressionPayload>() {
  return fast_expression_payloads_;
}
template <>
inline base::Arena<IfExpressionPayload>& Context::arena<IfExpressionPayload>() {
  return if_expression_payloads_;
}
template <>
inline base::Arena<LoopExpressionPayload>&
Context::arena<LoopExpressionPayload>() {
  return loop_expression_payloads_;
}
template <>
inline base::Arena<WhileExpressionPayload>&
Context::arena<WhileExpressionPayload>() {
  return while_expression_payloads_;
}
template <>
inline base::Arena<ForExpressionPayload>&
Context::arena<ForExpressionPayload>() {
  return for_expression_payloads_;
}
template <>
inline base::Arena<MatchExpressionPayload>&
Context::arena<MatchExpressionPayload>() {
  return match_expression_payloads_;
}
template <>
inline base::Arena<ClosureExpressionPayload>&
Context::arena<ClosureExpressionPayload>() {
  return closure_expression_payloads_;
}
template <>
inline base::Arena<AssignStatementPayload>&
Context::arena<AssignStatementPayload>() {
  return assign_statement_payloads_;
}
template <>
inline base::Arena<AttributeStatementPayload>&
Context::arena<AttributeStatementPayload>() {
  return attribute_statement_payloads_;
}
template <>
inline base::Arena<RedirectStatementPayload>&
Context::arena<RedirectStatementPayload>() {
  return redirect_statement_payloads_;
}
template <>
inline base::Arena<FunctionDeclarationPayload>&
Context::arena<FunctionDeclarationPayload>() {
  return function_declaration_payloads_;
}
template <>
inline base::Arena<StructDeclarationPayload>&
Context::arena<StructDeclarationPayload>() {
  return struct_declaration_payloads_;
}
template <>
inline base::Arena<EnumerationDeclarationPayload>&
Context::arena<EnumerationDeclarationPayload>() {
  return enumeration_declaration_payloads_;
}
template <>
inline base::Arena<TraitDeclarationPayload>&
Context::arena<TraitDeclarationPayload>() {
  return trait_declaration_payloads_;
}
template <>
inline base::Arena<ImplementationDeclarationPayload>&
Context::arena<ImplementationDeclarationPayload>() {
  return impl_declaration_payloads_;
}
template <>
inline base::Arena<UnionDeclarationPayload>&
Context::arena<UnionDeclarationPayload>() {
  return union_declaration_payloads_;
}
template <>
inline base::Arena<ModuleDeclarationPayload>&
Context::arena<ModuleDeclarationPayload>() {
  return module_declaration_payloads_;
}

template <>
inline base::Arena<CapturePayload>& Context::arena<CapturePayload>() {
  return capture_payloads_;
}
template <>
inline base::Arena<ParameterPayload>& Context::arena<ParameterPayload>() {
  return parameter_payloads_;
}
template <>
inline base::Arena<TypeReferencePayload>&
Context::arena<TypeReferencePayload>() {
  return type_reference_payloads_;
}
template <>
inline base::Arena<ArrayTypePayload>& Context::arena<ArrayTypePayload>() {
  return array_type_payloads_;
}
template <>
inline base::Arena<IdentifierPayload>& Context::arena<IdentifierPayload>() {
  return identifier_payloads_;
}
template <>
inline base::Arena<StorageAttributePayload>&
Context::arena<StorageAttributePayload>() {
  return storage_attribute_payloads_;
}
template <>
inline base::Arena<IfBranchPayload>& Context::arena<IfBranchPayload>() {
  return if_branch_payloads_;
}
template <>
inline base::Arena<MatchArmPayload>& Context::arena<MatchArmPayload>() {
  return match_arm_payloads_;
}
template <>
inline base::Arena<VariablePayload>& Context::arena<VariablePayload>() {
  return variable_payloads_;
}

template <>
inline PayloadId Context::alloc<NodeId>(NodeId&&) = delete;

}  // namespace ast

#endif  // FRONTEND_DATA_AST_CONTEXT_H_
