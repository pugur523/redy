// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_PAYLOAD_EXPRESSION_H_
#define FRONTEND_DATA_HIR_PAYLOAD_EXPRESSION_H_

#include <cstddef>

#include "core/base/source_range.h"
#include "frontend/base/keyword/type.h"
#include "frontend/base/literal/literal.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/base/string/string_interner.h"
#include "frontend/data/hir/base/node_id.h"
#include "frontend/data/hir/payload/common.h"

namespace hir {

struct LiteralExpressionPayload {
  base::LiteralType type;

  using int128_t = __int128_t;
  using uint128_t = __uint128_t;

  union Value {
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    int128_t i128;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    uint128_t u128;
    float f32;
    double f64;
    char32_t c;
    bool b;
    base::StringId str_id;

    Value() {}
    ~Value() {}
  } value;
};

struct ResolvedPathExpressionPayload {
  NodeId resolved_target;
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

struct CallExpressionPayload {
  NodeId callee;
  NodeRange args_range;
};

struct FieldAccessExpressionPayload {
  NodeId obj;
  PayloadId<ResolvedPathExpressionPayload> field;
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

struct IfExpressionPayload {
  PayloadRange<IfBranchPayload> branches_range;
};

struct WhileExpressionPayload {
  NodeId condition = kInvalidNodeId;
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

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_PAYLOAD_EXPRESSION_H_
