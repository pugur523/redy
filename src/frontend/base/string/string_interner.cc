// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/string/string_interner.h"

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>
#include <vector>

#include "core/check.h"
#include "frontend/base/string/string_id.h"

namespace base {

StringInterner::StringInterner(double max_load_factor)
    : max_load_factor_(max_load_factor) {
  // allocate 1 KiB string arena
  arena_.reserve(1024);
  // allocate 64 bytes small bucket table
  init_buckets(64);
}

StringInterner::~StringInterner() = default;

StringId StringInterner::intern(std::string_view s) {
  if (s.empty()) {
    return kEmptyStringId;
  }
  ensure_load_factor();

  const uint64_t hash = hash_sv(s);
  const uint32_t mask = static_cast<uint32_t>(buckets_.size() - 1);
  uint32_t idx = static_cast<uint32_t>(hash) & mask;

  while (true) {
    Bucket& b = buckets_[idx];
    if (b.empty()) {
      // insert new
      const StringId id = arena_.alloc(s);
      b = Bucket{hash, id};
      ++used_buckets_;
      return id;
    } else if (b.hash == hash && arena_[b.id] == s) {
      // already interned
      return b.id;
    }
    // linear probing
    idx = (idx + 1) & mask;
  }
}

std::string_view StringInterner::lookup(StringId id) const {
  return arena_[id];
}

StringId StringInterner::lookup(std::string_view s) const {
  if (s.empty()) {
    return kEmptyStringId;
  }
  const uint64_t hash = hash_sv(s);
  const uint32_t mask = static_cast<uint32_t>(buckets_.size() - 1);
  uint32_t idx = static_cast<uint32_t>(hash) & mask;

  for (std::size_t i = 0; i < buckets_.size(); ++i) {
    const Bucket& b = buckets_[idx];
    if (b.empty()) {
      return kInvalidStringId;
    } else if (b.hash == hash && arena_[b.id] == s) {
      return b.id;
    } else {
      idx = (idx + 1) & mask;
    }
  }
  return kInvalidStringId;
}

void StringInterner::init_buckets(std::size_t n) {
  // round up to next power of 2
  if ((n & (n - 1)) != 0) {
    --n;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    ++n;
  }

  buckets_.clear();
  buckets_.resize(n);
  used_buckets_ = 0;
}

void StringInterner::ensure_load_factor() {
  if (load_factor() > max_load_factor_) {
    rehash(buckets_.size() * 2);
  }
}

void StringInterner::rehash(std::size_t new_size) {
  // save old
  std::vector<Bucket> old = std::move(buckets_);
  init_buckets(new_size);

  // reinsert existing entries
  for (const auto& b : old) {
    if (b.id == kInvalidStringId) {
      continue;
    }
    const uint32_t mask = static_cast<uint32_t>(buckets_.size() - 1);
    uint32_t idx = static_cast<uint32_t>(b.hash) & mask;
    while (buckets_[idx].id != kInvalidStringId) {
      idx = (idx + 1) & mask;
    }
    buckets_[idx] = b;
    ++used_buckets_;
  }
}

}  // namespace base
