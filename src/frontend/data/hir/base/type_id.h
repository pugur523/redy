// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_BASE_TYPE_ID_H_
#define FRONTEND_DATA_HIR_BASE_TYPE_ID_H_

#include <cstdint>

namespace hir {

struct TypeId {
  uint32_t id;
};

constexpr const TypeId kInvalidTypeId = {0xFFFFFFFF};

struct TypeRange {
  TypeId begin = kInvalidTypeId;
  uint32_t size = 0;

  inline bool valid() const {
    return begin.id != kInvalidTypeId.id && size > 0;
  }
};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_BASE_TYPE_ID_H_
