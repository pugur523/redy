// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_PAYLOAD_COMMON_H_
#define FRONTEND_DATA_AST_PAYLOAD_COMMON_H_

#include <cstddef>

#include "frontend/base/data/payload_util.h"

namespace ast {

template <typename T>
using PayloadId = base::PayloadId<T>;

template <typename T>
using PayloadRange = base::PayloadRange<T>;

// forward declarations

// expression
struct LiteralExpressionPayload;
struct PathExpressionPayload;
struct UnaryExpressionPayload;
struct BinaryExpressionPayload;
struct GroupedExpressionPayload;
struct ArrayExpressionPayload;
struct TupleExpressionPayload;
struct IndexExpressionPayload;
struct ConstructExpressionPayload;
struct FunctionCallExpressionPayload;
struct MethodCallExpressionPayload;
struct FunctionMacroCallExpressionPayload;
struct MethodMacroCallExpressionPayload;
struct FieldAccessExpressionPayload;
struct AwaitExpressionPayload;
struct ContinueExpressionPayload;
struct BreakExpressionPayload;
struct RangeExpressionPayload;
struct ReturnExpressionPayload;
struct BlockExpressionPayload;
struct IfExpressionPayload;
struct LoopExpressionPayload;
struct WhileExpressionPayload;
struct ForExpressionPayload;
struct MatchExpressionPayload;
struct ClosureExpressionPayload;

// statements
struct AssignStatementPayload;
struct AttributeStatementPayload;
struct ExpressionStatementPayload;
struct UseStatementPayload;

struct FunctionDeclarationPayload;
struct StructDeclarationPayload;
struct EnumerationDeclarationPayload;
struct TraitDeclarationPayload;
struct ImplementationDeclarationPayload;
struct UnionDeclarationPayload;
struct ModuleDeclarationPayload;
struct RedirectDeclarationPayload;

// data
struct AttributeUsePayload;
struct CapturePayload;
struct FieldPayload;
struct ParameterPayload;
struct EnumVariantPayload;
struct TypeReferencePayload;
struct ArrayTypePayload;
struct IdentifierPayload;
struct IfBranchPayload;
struct MatchArmPayload;

// utility structures
struct StorageAttributeData;

}  // namespace ast

#endif  // FRONTEND_DATA_AST_PAYLOAD_COMMON_H_
