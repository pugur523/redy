// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_PAYLOAD_EXPRESSION_H_
#define FRONTEND_DATA_AST_PAYLOAD_EXPRESSION_H_

#include <cstddef>

#include "core/base/source_range.h"
#include "frontend/base/keyword/type.h"
#include "frontend/base/literal/literal.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/common.h"

namespace ast {

struct LiteralExpressionPayload {
  base::LiteralKind kind = base::LiteralKind::kUnknown;
  core::SourceRange lexeme_range;
};

struct PathExpressionPayload {
  PayloadRange<IdentifierPayload> path_parts_range;
  bool is_absolute : 1 = false;
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
  NodeId type_path;
  NodeRange args_range;
};

struct FunctionCallExpressionPayload {
  NodeId callee;
  NodeRange args_range;
};

struct MethodCallExpressionPayload {
  NodeId obj;
  PayloadId<PathExpressionPayload> method;
  NodeRange args_range;
};

struct FunctionMacroCallExpressionPayload {
  NodeId macro_callee;
  NodeRange args_range;
};

struct MethodMacroCallExpressionPayload {
  NodeId obj;
  PayloadId<PathExpressionPayload> macro_method;
  NodeRange args_range;
};

struct FieldAccessExpressionPayload {
  NodeId obj;
  PayloadId<PathExpressionPayload> field;
};

struct AwaitExpressionPayload {
  NodeId callee_expression = kInvalidNodeId;
};

struct ContinueExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct BreakExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct RangeExpressionPayload {
  NodeId begin = kInvalidNodeId;
  NodeId end = kInvalidNodeId;
  bool is_exclusive : 1 = true;
};

struct ReturnExpressionPayload {
  NodeId expression = kInvalidNodeId;
};

struct BlockExpressionPayload {
  NodeRange body_nodes_range;
};

struct UnsafeExpressionPayload {
  PayloadId<BlockExpressionPayload> block;
};

struct FastExpressionPayload {
  PayloadId<BlockExpressionPayload> block;
};

struct IfExpressionPayload {
  PayloadRange<IfBranchPayload> branches_range;
};

struct LoopExpressionPayload {
  PayloadId<BlockExpressionPayload> body;
};

struct WhileExpressionPayload {
  NodeId condition = kInvalidNodeId;
  PayloadId<BlockExpressionPayload> body;
};

struct ForExpressionPayload {
  PayloadId<PathExpressionPayload> iterator;
  NodeId range = kInvalidNodeId;
  PayloadId<BlockExpressionPayload> body;
};

struct MatchExpressionPayload {
  NodeId expression = kInvalidNodeId;
  PayloadRange<MatchArmPayload> arms_range;
};

struct ClosureExpressionPayload {
  PayloadRange<CapturePayload> captures_range;
  PayloadRange<ParameterPayload> parameters_range;
  NodeId body;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_PAYLOAD_EXPRESSION_H_
