// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_PAYLOAD_COMMON_H_
#define FRONTEND_DATA_AST_PAYLOAD_COMMON_H_

#include <cstddef>
#include <limits>

namespace ast {

constexpr uint32_t kInvalidPayloadId = std::numeric_limits<uint32_t>::max();

template <typename T>
struct PayloadId {
  uint32_t id = kInvalidPayloadId;

  constexpr PayloadId() = default;
  constexpr ~PayloadId() = default;
  constexpr explicit PayloadId(std::size_t n) : id(n) {}
  constexpr PayloadId operator=(std::size_t n) { return PayloadId{n}; }

  inline bool valid() const { return id != kInvalidPayloadId; }
};

template <typename T>
struct PayloadRange {
  PayloadId<T> begin;
  uint32_t size = 0;

  inline bool valid() const { return begin.valid() && size > 0; }
};

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
struct UnsafeExpressionPayload;
struct FastExpressionPayload;
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
