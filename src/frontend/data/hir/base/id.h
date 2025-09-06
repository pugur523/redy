// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_BASE_ID_H_
#define FRONTEND_DATA_HIR_BASE_ID_H_

#include <cstdint>

namespace hir {

struct DefId {
  uint64_t id;
};

struct HirId {
  uint32_t id;
};

struct LocalId {
  uint32_t id;
};

constexpr DefId kInvalidDefId = {0xFFFFFFFF};
constexpr HirId kInvalidHirId = {0xFFFFFFFF};
constexpr LocalId kInvalidLocalId = {0xFFFFFFFF};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_BASE_ID_H_
