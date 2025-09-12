// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_BASE_NODE_ID_H_
#define FRONTEND_DATA_HIR_BASE_NODE_ID_H_

#include <cstdint>

namespace hir {

struct NodeId {
  uint32_t id;
};

constexpr const NodeId kInvalidNodeId = {0xFFFFFFFF};

struct NodeRange {
  NodeId begin = kInvalidNodeId;
  uint32_t size = 0;

  inline bool valid() const {
    return begin.id != kInvalidNodeId.id && size > 0;
  }
};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_BASE_NODE_ID_H_
