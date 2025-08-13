// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODE_ID_H_
#define FRONTEND_DATA_AST_BASE_NODE_ID_H_

#include <cstdint>
#include <limits>

namespace ast {

using NodeId = std::size_t;

constexpr const NodeId kInvalidNodeId = std::numeric_limits<NodeId>::max();

// template <typename T>
// struct NodeId {
//   std::size_t id = std::numeric_limits<std::size_t>::max();
// };

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_ID_H_
