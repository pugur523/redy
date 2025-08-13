// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODES_H_
#define FRONTEND_DATA_AST_BASE_NODES_H_

#include <cstddef>
#include <limits>
#include <string_view>

#include "frontend/base/literal/literal.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"

namespace ast {

// utilities
struct NodeRange {
  NodeId begin = kInvalidNodeId;
  uint32_t size = 0;
};

struct TypeReferenceNode {
  TypeKind type_kind = TypeKind::kAuto;
};

// struct ParameterNode {
//   std::string_view name = "";
//   NodeId type_path = kInvalidNodeId;
// };
//
// struct FieldInitializerNode {
//   std::string_view name = "";
//   NodeId value = kInvalidNodeId;
// };

// expressions

struct LiteralExpressionNode {
  base::LiteralKind kind = base::LiteralKind::kUnknown;
  std::string_view value = "";
};

struct PathExpressionNode {
  std::string_view path = "";
};

struct UnaryOperatorExpressionNode {
  base::UnaryOperator op = base::UnaryOperator::kUnknown;
  NodeId operand = kInvalidNodeId;
};

struct BinaryOperatorExpressionNode {
  base::BinaryOperator op = base::BinaryOperator::kUnknown;
  NodeId lhs = kInvalidNodeId;
  NodeId rhs = kInvalidNodeId;
};

struct GroupedExpressionNode {
  NodeId expression = kInvalidNodeId;
};

struct ArrayExpressionNode {
  NodeRange array_elements_range;
};

struct TupleExpressionNode {
  NodeRange tuple_elements_range;
};

struct IndexExpressionNode {
  NodeId operand = kInvalidNodeId;
  NodeId index = kInvalidNodeId;
};

struct ConstructExpressionNode {
  NodeId type_path = kInvalidNodeId;
  NodeRange args_range;
};

struct FunctionCallExpressionNode {
  NodeId callee = kInvalidNodeId;
  NodeRange args_range;
};

struct MethodCallExpressionNode {
  NodeId receiver = kInvalidNodeId;
  NodeId method_callee = kInvalidNodeId;
  NodeRange args_range;
};

struct MacroCallExpressionNode {
  NodeId macro_path = kInvalidNodeId;
};

struct FieldAccessExpressionNode {
  NodeId base = kInvalidNodeId;
  NodeId field = kInvalidNodeId;
};

struct AwaitExpressionNode {
  NodeId expression = kInvalidNodeId;
};

struct ContinueExpressionNode {
  NodeId expression = kInvalidNodeId;
};

struct BreakExpressionNode {
  NodeId expression = kInvalidNodeId;
};

struct ExclusiveRangeExpressionNode {
  NodeId begin = kInvalidNodeId;
  NodeId end = kInvalidNodeId;
};

struct InclusiveRangeExpressionNode {
  NodeId begin = kInvalidNodeId;
  NodeId end = kInvalidNodeId;
};

struct ReturnExpressionNode {
  NodeId value = kInvalidNodeId;
};

struct BlockExpressionNode {
  NodeRange body_nodes_range;
};

struct ConstBlockExpressionNode {
  NodeId block;
};

struct UnsafeExpressionNode {
  NodeId block;
};

struct FastExpressionNode {
  NodeId block;
};

struct IfExpressionNode {
  NodeId condition = kInvalidNodeId;
  NodeId then_block = kInvalidNodeId;
  NodeId else_block = kInvalidNodeId;
};

struct LoopExpressionNode {
  NodeId body = kInvalidNodeId;
};

struct WhileExpressionNode {
  NodeId condition = kInvalidNodeId;
  NodeId body = kInvalidNodeId;
};

struct ForExpressionNode {
  NodeId iterator = kInvalidNodeId;
  NodeId body = kInvalidNodeId;
};

struct MatchExpressionNode {
  NodeId expression = kInvalidNodeId;
  NodeRange arms_range;
};

struct ClosureExpressionNode {
  NodeRange parameters_range;
  NodeId body = kInvalidNodeId;
};

// statements

struct AssignStatementNode {
  NodeId target = kInvalidNodeId;
  NodeId value = kInvalidNodeId;
};

struct ConstAssignStatementNode {
  NodeId target = kInvalidNodeId;
  NodeId value = kInvalidNodeId;
};

struct AttributeStatementNode {
  NodeRange attributes_range;
};

struct RedirectStatementNode {
  NodeId target = kInvalidNodeId;
  NodeId source = kInvalidNodeId;
};

struct ExpressionStatementNode {
  NodeId expression;
};

// declaration

struct FunctionDeclarationNode {
  std::string_view name = "";
  NodeRange parameters_range;
  NodeId return_type = kInvalidNodeId;
  NodeId body = kInvalidNodeId;
};

struct StructDeclarationNode {
  std::string_view name = "";
  NodeRange fields_range;
};

struct EnumerationDeclarationNode {
  std::string_view name = "";
  NodeRange variants_range;
};

struct UnionDeclarationNode {
  std::string_view name = "";
  NodeRange fields_range;
};

struct ModuleDeclarationNode {
  std::string_view name = "";
  NodeRange module_nodes_range;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODES_H_
