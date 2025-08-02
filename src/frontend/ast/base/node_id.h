// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_BASE_NODE_ID_H_
#define FRONTEND_AST_BASE_NODE_ID_H_

#include <cstdint>

namespace ast {

template <typename T>
struct NodeId {
  uint32_t index;
};

}  // namespace ast

#endif  // FRONTEND_AST_BASE_NODE_ID_H_
