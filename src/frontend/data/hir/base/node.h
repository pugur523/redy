// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_BASE_NODE_H_
#define FRONTEND_DATA_HIR_BASE_NODE_H_

#include <cstddef>
#include <cstdint>

#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"

namespace hir {

enum class NodeKind : uint8_t {
  kUnknown = 0,

  kAssignStatement = 1,
  kAttributeStatement = 2,
  // use statement is removed: paths will be resolved on this phase, no need
  // to leave it any more

  kLiteralExpression = 3,
  kResolvedPathExpression = 4,
  kUnaryExpression = 5,
  kBinaryExpression = 6,
  kArrayExpression = 7,
  kTupleExpression = 8,
  kIndexExpression = 9,
  kConstructExpression = 10,
  // function, method, macro call will be call
  kCallExpression = 11,
  kFieldAccessExpression = 12,
  kAwaitExpression = 13,
  kContinueExpression = 14,
  kBreakExpression = 15,
  kRangeExpression = 16,
  kReturnExpression = 17,
  kBlockExpression = 18,
  kIfExpression = 19,
  // for and loop will be while
  kWhileExpression = 20,
  kMatchExpression = 21,
  kClosureExpression = 22,

  // impl and redirect will be function
  kFunctionDeclaration = 23,
  kStructDeclaration = 24,
  kEnumDeclaration = 25,
  kUnionDeclaration = 26,
  kTraitDeclaration = 27,
  kModuleDeclaration = 28,
  kGlobalVariableDeclaration = 29,
};

struct Node {
  uint32_t payload_id = 0xFFFFFFFF;
  NodeKind kind = NodeKind::kUnknown;
};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_BASE_NODE_H_
