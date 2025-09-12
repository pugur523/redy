// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_PAYLOAD_COMMON_H_
#define FRONTEND_DATA_HIR_PAYLOAD_COMMON_H_

#include "core/base/source_range.h"
#include "frontend/base/data/payload_util.h"
#include "frontend/base/keyword/type.h"
#include "frontend/base/literal/literal.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/data/hir/base/node_id.h"

namespace hir {

template <typename T>
using PayloadId = base::PayloadId<T>;

template <typename T>
using PayloadRange = base::PayloadRange<T>;

// forward declarations

// expression
struct LiteralExpressionPayload;
struct ResolvedPathExpressionPayload;
struct UnaryExpressionPayload;
struct BinaryExpressionPayload;
struct ArrayExpressionPayload;
struct TupleExpressionPayload;
struct IndexExpressionPayload;
struct ConstructExpressionPayload;
struct CallExpressionPayload;
struct FieldAccessExpressionPayload;
struct AwaitExpressionPayload;
struct ContinueExpressionPayload;
struct BreakExpressionPayload;
struct RangeExpressionPayload;
struct ReturnExpressionPayload;
struct BlockExpressionPayload;
struct IfExpressionPayload;
struct WhileExpressionPayload;
struct MatchExpressionPayload;
struct ClosureExpressionPayload;

// statement
struct AssignStatementPayload;
struct AttributeStatementPayload;

struct FunctionDeclarationPayload;
struct StructDeclarationPayload;
struct EnumerationDeclarationPayload;
struct TraitDeclarationPayload;
struct UnionDeclarationPayload;
struct ModuleDeclarationPayload;
struct GlobalVariableDeclarationPayload;

// data
struct FunctionSignaturePayload;
struct AttributeUsePayload;
struct CapturePayload;
struct FieldPayload;
struct ParameterPayload;
struct EnumVariantPayload;
struct IfBranchPayload;
struct MatchArmPayload;

// utility structure
struct StorageAttributeData;

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_PAYLOAD_COMMON_H_
