// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODES_H_
#define FRONTEND_DATA_AST_BASE_NODES_H_

#include <cstddef>
#include <limits>
#include <string_view>

#include "frontend/base/keyword/type.h"
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
  std::string_view type_name = "";
  base::TypeKind type_kind = base::TypeKind::kAuto;
};

struct BuiltinAttribute {
  bool is_mutable : 1 = false;
  bool is_const : 1 = false;
  bool is_extern : 1 = false;
  bool is_static : 1 = false;
  bool is_thread_local : 1 = false;
  bool is_deprecated : 1 = false;
  bool is_public : 1 = false;
};

// keep `Attribute` as a 1 byte or cast it to a 2 byte type such as uint16_t
inline bool has_any_builtin_attribute(BuiltinAttribute attribute) {
  return *reinterpret_cast<uint8_t*>(&attribute) != 0;
}

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
  BuiltinAttribute attribute;
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
  BuiltinAttribute attribute;
};

struct StructDeclarationNode {
  std::string_view name = "";
  NodeRange fields_range;
  BuiltinAttribute attribute;
};

struct EnumerationDeclarationNode {
  std::string_view name = "";
  NodeRange variants_range;
  BuiltinAttribute attribute;
};

struct TraitDeclarationNode {
  std::string_view name = "";
  NodeRange function_declare_range;
  BuiltinAttribute attribute;
};

struct ImplementationDeclarationNode {
  std::string_view name = "";
  NodeRange function_definition_range;
  BuiltinAttribute attribute;
};

struct UnionDeclarationNode {
  std::string_view name = "";
  NodeRange fields_range;
  BuiltinAttribute attribute;
};

struct ModuleDeclarationNode {
  std::string_view name = "";
  NodeRange module_nodes_range;
  BuiltinAttribute attribute;
};

// chore

struct ParameterNode {
  std::string_view name = "";
  NodeId type = kInvalidNodeId;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODES_H_
