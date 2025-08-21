// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODE_ID_H_
#define FRONTEND_DATA_AST_BASE_NODE_ID_H_

#include <cstdint>
#include <limits>

namespace ast {

using NodeId = std::size_t;
using PayloadId = std::size_t;
using TokenId = std::size_t;

constexpr NodeId kInvalidNodeId = std::numeric_limits<NodeId>::max();
constexpr PayloadId kInvalidPayloadId = std::numeric_limits<PayloadId>::max();
constexpr TokenId kInvalidTokenId = std::numeric_limits<TokenId>::max();

struct NodeRange {
  NodeId begin = kInvalidNodeId;
  uint32_t size = 0;
};

struct PayloadRange {
  PayloadId begin = kInvalidPayloadId;
  uint32_t size = 0;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_ID_H_
