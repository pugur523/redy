// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_BASE_NODE_H_
#define FRONTEND_DATA_HIR_BASE_NODE_H_

#include <cstddef>
#include <cstdint>

#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/hir/payload/common.h"

namespace hir {

enum class HirNodeKind : uint8_t {
  kUnknown = 0,
  kExpression = 1,
  kStatement = 2,
  kPattern = 3,
  // TODO add more
};

struct HirNode {
  uint32_t payload_id = 0xFFFFFFFF;
  HirNodeKind kind = HirNodeKind::kUnknown;
};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_BASE_NODE_H_
