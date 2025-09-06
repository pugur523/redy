// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/string/string_interner.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string_view>
#include <utility>
#include <vector>

namespace base {

StringInterner::StringInterner() {
  // allocate 1 KiB string arena
  arena_.reserve(1024);
  // allocate 64 bytes small bucket table
  init_buckets(64);
}

StringInterner::~StringInterner() = default;

StringId StringInterner::intern(std::string_view s) {
  if (s.empty()) {
    // treat empty as invalid
    return kInvalidStringId;
  }
  ensure_load_factor();

  const uint64_t hash = hash_sv(s);
  const uint32_t mask = static_cast<uint32_t>(buckets_.size() - 1);
  uint32_t idx = static_cast<uint32_t>(hash) & mask;

  while (true) {
    Bucket& b = buckets_[idx];
    if (b.id == kEmptyId) {
      // insert new
      const StringId id = arena_.alloc(s);
      b = Bucket{hash, id};
      return id;
    } else if (b.hash == hash) {
      // potential match; compare bytes
      if (arena_[b.id] == s) {
        // already interned
        return b.id;
      }
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
    return 0;
  }
  const uint64_t hash = hash_sv(s);
  const uint32_t mask = static_cast<uint32_t>(buckets_.size() - 1);
  uint32_t idx = static_cast<uint32_t>(hash) & mask;

  for (std::size_t i = 0; i < buckets_.size(); ++i) {
    const Bucket& b = buckets_[idx];
    if (b.id == kEmptyId) {
      return kInvalidStringId;
    } else if (b.hash == hash && arena_[b.id] == s) {
      return b.id;
    }
    idx = (idx + 1) & mask;
  }
  return kInvalidStringId;
}

void StringInterner::init_buckets(std::size_t n) {
  // n must be power of two
  buckets_.clear();
  buckets_.resize(n);
}

void StringInterner::ensure_load_factor() {
  const double current_load_factor =
      static_cast<double>(arena_.size()) / static_cast<double>(buckets_.size());
  if (current_load_factor > load_factor_) {
    rehash(buckets_.size() * 2);
  }
}

void StringInterner::rehash(std::size_t new_size) {
  // save old
  std::vector<Bucket> old = std::move(buckets_);
  init_buckets(new_size);

  // reinsert existing entries
  for (const auto& b : old) {
    if (b.id == kEmptyId) {
      continue;
    }
    const uint32_t mask = static_cast<uint32_t>(buckets_.size() - 1);
    uint32_t idx = static_cast<uint32_t>(b.hash) & mask;
    while (buckets_[idx].id != kEmptyId) {
      idx = (idx + 1) & mask;
    }
    buckets_[idx] = b;
  }
}

}  // namespace base
