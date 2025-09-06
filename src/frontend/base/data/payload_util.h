// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_DATA_PAYLOAD_UTIL_H_
#define FRONTEND_BASE_DATA_PAYLOAD_UTIL_H_

#include <cstddef>
#include <cstdint>

namespace base {

constexpr uint32_t kInvalidPayloadId = 0xFFFFFFFF;

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

}  // namespace base

#endif  // FRONTEND_BASE_DATA_PAYLOAD_UTIL_H_
