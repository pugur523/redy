// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODE_ID_H_
#define FRONTEND_DATA_AST_BASE_NODE_ID_H_

#include <cstdint>
#include <limits>

namespace ast {

using NodeId = uint32_t;
using TokenId = uint32_t;

constexpr NodeId kInvalidNodeId = std::numeric_limits<NodeId>::max();
constexpr TokenId kInvalidTokenId = std::numeric_limits<TokenId>::max();

struct NodeRange {
  NodeId begin = kInvalidNodeId;
  uint32_t size = 0;

  inline bool valid() const { return begin != kInvalidNodeId && size > 0; }
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_ID_H_
