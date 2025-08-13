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
#include "frontend/data/ast/base/nodes.h"

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
  inline NodeId alloc(const T& value) {
    return arena<T>().alloc(value);
  }

  template <typename T>
  inline T& get(NodeId id) {
    return arena<T>()[id];
  }

 private:
  Context() = default;

  base::Arena<NodeHeader> node_headers_;

  base::Arena<TypeReferenceNode> type_reference_nodes_;

  base::Arena<LiteralExpressionNode> literal_expression_nodes_;
  base::Arena<PathExpressionNode> path_expression_nodes_;
  base::Arena<UnaryOperatorExpressionNode> unary_operator_expression_nodes_;
  base::Arena<BinaryOperatorExpressionNode> binary_operator_expression_nodes_;
  base::Arena<GroupedExpressionNode> grouped_expression_nodes_;
  base::Arena<ArrayExpressionNode> array_expression_nodes_;
  base::Arena<TupleExpressionNode> tuple_expression_nodes_;
  base::Arena<IndexExpressionNode> index_expression_nodes_;
  base::Arena<ConstructExpressionNode> construct_expression_nodes_;
  base::Arena<FunctionCallExpressionNode> function_call_expression_nodes_;
  base::Arena<MethodCallExpressionNode> method_call_expression_nodes_;
  base::Arena<MacroCallExpressionNode> macro_call_expression_nodes_;
  base::Arena<FieldAccessExpressionNode> field_access_expression_nodes_;
  base::Arena<AwaitExpressionNode> await_expression_nodes_;
  base::Arena<ContinueExpressionNode> continue_expression_nodes_;
  base::Arena<BreakExpressionNode> break_expression_nodes_;
  base::Arena<ExclusiveRangeExpressionNode> exclusive_range_expression_nodes_;
  base::Arena<InclusiveRangeExpressionNode> inclusive_range_expression_nodes_;
  base::Arena<ReturnExpressionNode> return_expression_nodes_;

  base::Arena<BlockExpressionNode> block_expression_nodes_;
  base::Arena<ConstBlockExpressionNode> const_block_expression_nodes_;
  base::Arena<UnsafeExpressionNode> unsafe_expression_nodes_;
  base::Arena<FastExpressionNode> fast_expression_nodes_;
  base::Arena<IfExpressionNode> if_expression_nodes_;
  base::Arena<LoopExpressionNode> loop_expression_nodes_;
  base::Arena<WhileExpressionNode> while_expression_nodes_;
  base::Arena<ForExpressionNode> for_expression_nodes_;
  base::Arena<MatchExpressionNode> match_expression_nodes_;
  base::Arena<ClosureExpressionNode> closure_expression_nodes_;

  base::Arena<AssignStatementNode> assign_statement_nodes_;
  base::Arena<ConstAssignStatementNode> const_assign_statement_nodes_;
  base::Arena<AttributeStatementNode> attribute_statement_nodes_;
  base::Arena<RedirectStatementNode> redirect_statement_nodes_;

  base::Arena<FunctionDeclarationNode> function_declaration_nodes_;
  base::Arena<StructDeclarationNode> struct_declaration_nodes_;
  base::Arena<EnumerationDeclarationNode> enumeration_declaration_nodes_;
  base::Arena<UnionDeclarationNode> union_declaration_nodes_;
  base::Arena<ModuleDeclarationNode> module_declaration_nodes_;
};

// arena<T>() specializations
template <>
inline base::Arena<NodeHeader>& Context::arena<NodeHeader>() {
  return node_headers_;
}

template <>
inline base::Arena<TypeReferenceNode>& Context::arena<TypeReferenceNode>() {
  return type_reference_nodes_;
}

template <>
inline base::Arena<LiteralExpressionNode>&
Context::arena<LiteralExpressionNode>() {
  return literal_expression_nodes_;
}
template <>
inline base::Arena<PathExpressionNode>& Context::arena<PathExpressionNode>() {
  return path_expression_nodes_;
}
template <>
inline base::Arena<UnaryOperatorExpressionNode>&
Context::arena<UnaryOperatorExpressionNode>() {
  return unary_operator_expression_nodes_;
}
template <>
inline base::Arena<BinaryOperatorExpressionNode>&
Context::arena<BinaryOperatorExpressionNode>() {
  return binary_operator_expression_nodes_;
}
template <>
inline base::Arena<GroupedExpressionNode>&
Context::arena<GroupedExpressionNode>() {
  return grouped_expression_nodes_;
}
template <>
inline base::Arena<ArrayExpressionNode>& Context::arena<ArrayExpressionNode>() {
  return array_expression_nodes_;
}
template <>
inline base::Arena<TupleExpressionNode>& Context::arena<TupleExpressionNode>() {
  return tuple_expression_nodes_;
}
template <>
inline base::Arena<IndexExpressionNode>& Context::arena<IndexExpressionNode>() {
  return index_expression_nodes_;
}
template <>
inline base::Arena<ConstructExpressionNode>&
Context::arena<ConstructExpressionNode>() {
  return construct_expression_nodes_;
}
template <>
inline base::Arena<FunctionCallExpressionNode>&
Context::arena<FunctionCallExpressionNode>() {
  return function_call_expression_nodes_;
}
template <>
inline base::Arena<MethodCallExpressionNode>&
Context::arena<MethodCallExpressionNode>() {
  return method_call_expression_nodes_;
}
template <>
inline base::Arena<MacroCallExpressionNode>&
Context::arena<MacroCallExpressionNode>() {
  return macro_call_expression_nodes_;
}
template <>
inline base::Arena<FieldAccessExpressionNode>&
Context::arena<FieldAccessExpressionNode>() {
  return field_access_expression_nodes_;
}
template <>
inline base::Arena<AwaitExpressionNode>& Context::arena<AwaitExpressionNode>() {
  return await_expression_nodes_;
}
template <>
inline base::Arena<ContinueExpressionNode>&
Context::arena<ContinueExpressionNode>() {
  return continue_expression_nodes_;
}
template <>
inline base::Arena<BreakExpressionNode>& Context::arena<BreakExpressionNode>() {
  return break_expression_nodes_;
}
template <>
inline base::Arena<ExclusiveRangeExpressionNode>&
Context::arena<ExclusiveRangeExpressionNode>() {
  return exclusive_range_expression_nodes_;
}
template <>
inline base::Arena<InclusiveRangeExpressionNode>&
Context::arena<InclusiveRangeExpressionNode>() {
  return inclusive_range_expression_nodes_;
}
template <>
inline base::Arena<ReturnExpressionNode>&
Context::arena<ReturnExpressionNode>() {
  return return_expression_nodes_;
}
template <>
inline base::Arena<BlockExpressionNode>& Context::arena<BlockExpressionNode>() {
  return block_expression_nodes_;
}
template <>
inline base::Arena<ConstBlockExpressionNode>&
Context::arena<ConstBlockExpressionNode>() {
  return const_block_expression_nodes_;
}
template <>
inline base::Arena<UnsafeExpressionNode>&
Context::arena<UnsafeExpressionNode>() {
  return unsafe_expression_nodes_;
}
template <>
inline base::Arena<FastExpressionNode>& Context::arena<FastExpressionNode>() {
  return fast_expression_nodes_;
}
template <>
inline base::Arena<IfExpressionNode>& Context::arena<IfExpressionNode>() {
  return if_expression_nodes_;
}
template <>
inline base::Arena<LoopExpressionNode>& Context::arena<LoopExpressionNode>() {
  return loop_expression_nodes_;
}
template <>
inline base::Arena<WhileExpressionNode>& Context::arena<WhileExpressionNode>() {
  return while_expression_nodes_;
}
template <>
inline base::Arena<ForExpressionNode>& Context::arena<ForExpressionNode>() {
  return for_expression_nodes_;
}
template <>
inline base::Arena<MatchExpressionNode>& Context::arena<MatchExpressionNode>() {
  return match_expression_nodes_;
}
template <>
inline base::Arena<ClosureExpressionNode>&
Context::arena<ClosureExpressionNode>() {
  return closure_expression_nodes_;
}
template <>
inline base::Arena<AssignStatementNode>& Context::arena<AssignStatementNode>() {
  return assign_statement_nodes_;
}
template <>
inline base::Arena<ConstAssignStatementNode>&
Context::arena<ConstAssignStatementNode>() {
  return const_assign_statement_nodes_;
}
template <>
inline base::Arena<AttributeStatementNode>&
Context::arena<AttributeStatementNode>() {
  return attribute_statement_nodes_;
}
template <>
inline base::Arena<RedirectStatementNode>&
Context::arena<RedirectStatementNode>() {
  return redirect_statement_nodes_;
}
template <>
inline base::Arena<FunctionDeclarationNode>&
Context::arena<FunctionDeclarationNode>() {
  return function_declaration_nodes_;
}
template <>
inline base::Arena<StructDeclarationNode>&
Context::arena<StructDeclarationNode>() {
  return struct_declaration_nodes_;
}
template <>
inline base::Arena<EnumerationDeclarationNode>&
Context::arena<EnumerationDeclarationNode>() {
  return enumeration_declaration_nodes_;
}
template <>
inline base::Arena<UnionDeclarationNode>&
Context::arena<UnionDeclarationNode>() {
  return union_declaration_nodes_;
}
template <>
inline base::Arena<ModuleDeclarationNode>&
Context::arena<ModuleDeclarationNode>() {
  return module_declaration_nodes_;
}

}  // namespace ast

#endif  // FRONTEND_DATA_AST_CONTEXT_H_
