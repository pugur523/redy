// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_PAYLOAD_H_
#define FRONTEND_DATA_AST_BASE_PAYLOAD_H_

#include <cstddef>
#include <limits>
#include <string_view>

#include "frontend/base/keyword/type.h"
#include "frontend/base/literal/literal.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/data/ast/base/node_id.h"

namespace ast {

// expressions

struct LiteralExpressionPayload {
  base::LiteralKind kind = base::LiteralKind::kUnknown;
  std::string_view lexeme = "";
};

struct PathExpressionPayload {
  PayloadRange path_parts_range;
};

struct UnaryExpressionPayload {
  base::UnaryOperator op = base::UnaryOperator::kUnknown;
  NodeId operand = kInvalidNodeId;
};

struct BinaryExpressionPayload {
  base::BinaryOperator op = base::BinaryOperator::kUnknown;
  NodeId lhs = kInvalidNodeId;
  NodeId rhs = kInvalidNodeId;
};

struct GroupedExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct ArrayExpressionPayload {
  NodeRange array_elements_range;
};

struct TupleExpressionPayload {
  NodeRange tuple_elements_range;
};

struct IndexExpressionPayload {
  NodeId operand = kInvalidNodeId;
  NodeId index = kInvalidNodeId;
};

struct ConstructExpressionPayload {
  NodeId type_path = kInvalidNodeId;
  PayloadRange args_range;
};

struct FunctionCallExpressionPayload {
  NodeId callee = kInvalidNodeId;
  NodeRange args_range;
};

struct MethodCallExpressionPayload {
  NodeId obj = kInvalidNodeId;
  PayloadId method = kInvalidNodeId;
  NodeRange args_range;
};

struct FunctionMacroCallExpressionPayload {
  NodeId macro_callee = kInvalidNodeId;
  NodeRange args_range;
};

struct MethodMacroCallExpressionPayload {
  NodeId obj = kInvalidNodeId;
  PayloadId macro_method = kInvalidNodeId;
  NodeRange args_range;
};

struct FieldAccessExpressionPayload {
  NodeId obj = kInvalidNodeId;
  PayloadId field = kInvalidNodeId;
};

struct AwaitExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct ContinueExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct BreakExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct ExclusiveRangeExpressionPayload {
  NodeId begin = kInvalidNodeId;
  NodeId end = kInvalidNodeId;
};

struct InclusiveRangeExpressionPayload {
  NodeId begin = kInvalidNodeId;
  NodeId end = kInvalidNodeId;
};

struct ReturnExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct BlockExpressionPayload {
  NodeRange body_nodes_range;
};

struct UnsafeExpressionPayload {
  NodeId block;
};

struct FastExpressionPayload {
  NodeId block;
};

struct IfExpressionPayload {
  PayloadRange branches_range;
};

struct LoopExpressionPayload {
  NodeId body = kInvalidNodeId;
};

struct WhileExpressionPayload {
  NodeId condition = kInvalidNodeId;
  NodeId body = kInvalidNodeId;
};

struct ForExpressionPayload {
  PayloadId iterator = kInvalidNodeId;
  NodeId range = kInvalidNodeId;
  NodeId body = kInvalidNodeId;
};

struct MatchExpressionPayload {
  NodeId expression = kInvalidNodeId;
  PayloadRange arms_range;
};

struct ClosureExpressionPayload {
  PayloadRange captures_range;
  PayloadRange parameters_range;
  NodeId body = kInvalidNodeId;
};

// statements

struct AssignStatementPayload {
  PayloadId target_variable = kInvalidPayloadId;
  NodeId value_expression = kInvalidNodeId;
  NodeId storage_attribute;
};

struct AttributeStatementPayload {
  NodeRange attributes_range;
};

struct RedirectStatementPayload {
  NodeId target = kInvalidNodeId;
  NodeId source = kInvalidNodeId;
};

struct ExpressionStatementPayload {
  NodeId expression;
};

// declaration

struct FunctionDeclarationPayload {
  std::string_view name = "";
  PayloadRange parameters_range;
  NodeId return_type = kInvalidNodeId;
  NodeId body = kInvalidNodeId;
  NodeId storage_attribute;
};

struct StructDeclarationPayload {
  std::string_view name = "";
  NodeRange fields_range;
  NodeId storage_attribute;
};

struct EnumerationDeclarationPayload {
  std::string_view name = "";
  NodeRange variants_range;
  NodeId storage_attribute;
};

struct TraitDeclarationPayload {
  std::string_view name = "";
  NodeRange function_declare_range;
  NodeId storage_attribute;
};

struct ImplementationDeclarationPayload {
  std::string_view name = "";
  NodeRange function_definition_range;
  NodeId storage_attribute;
};

struct UnionDeclarationPayload {
  std::string_view name = "";
  NodeRange fields_range;
  NodeId storage_attribute;
};

struct ModuleDeclarationPayload {
  std::string_view name = "";
  NodeRange module_nodes_range;
  NodeId storage_attribute;
};

// data

struct CapturePayload {
  std::string_view name = "";
  NodeId type = kInvalidNodeId;
};

struct ParameterPayload {
  std::string_view name = "";
  NodeId type = kInvalidNodeId;
};

struct TypeReferencePayload {
  std::string_view type_name = "";
  base::TypeKind type_kind = base::TypeKind::kAuto;
};

struct ArrayTypePayload {
  NodeId type = kInvalidNodeId;
  NodeId array_size = kInvalidNodeId;
};

struct IdentifierPayload {
  std::string_view lexeme = "";
};

struct StorageAttributePayload {
  bool is_mutable : 1 = false;
  bool is_const : 1 = false;
  bool is_extern : 1 = false;
  bool is_static : 1 = false;
  bool is_thread_local : 1 = false;
  bool is_public : 1 = false;
  bool is_async : 1 = false;

  inline bool has_any_storage_attribute() const {
    return *reinterpret_cast<const uint8_t*>(this) != 0;
  }
};

struct IfBranchPayload {
  NodeId condition = kInvalidNodeId;
  NodeId block = kInvalidNodeId;
};

struct MatchArmPayload {
  NodeId pattern = kInvalidNodeId;
  NodeId expression = kInvalidNodeId;
};

struct VariablePayload {
  PayloadId identifier_id = kInvalidPayloadId;
  PayloadId type_id = kInvalidPayloadId;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_PAYLOAD_H_
