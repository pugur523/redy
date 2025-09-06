// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_STRING_STRING_INTERNER_H_
#define FRONTEND_BASE_STRING_STRING_INTERNER_H_

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string_view>
#include <utility>
#include <vector>

#include "core/check.h"
#include "frontend/base/base_export.h"
#include "frontend/base/data/string_arena.h"

namespace base {

using StringId = uint32_t;
static constexpr const StringId kInvalidStringId = 0;

class BASE_EXPORT StringInterner {
 public:
  StringInterner();
  ~StringInterner();

  StringInterner(const StringInterner&) = delete;
  StringInterner& operator=(const StringInterner&) = delete;

  StringInterner(StringInterner&&) noexcept = default;
  StringInterner& operator=(StringInterner&&) noexcept = default;

  // interns a string and returns a stable id
  // if already present, returns existing id
  // average O(1)
  StringId intern(std::string_view s);
  std::string_view lookup(StringId id) const;
  StringId lookup(std::string_view s) const;

  inline std::size_t size() const { return arena_.size(); }

 private:
  void init_buckets(std::size_t n);
  void ensure_load_factor();
  void rehash(std::size_t new_size);

  // TODO: replace this with xxh3 or another fast and safe algorithm
  static inline uint64_t hash_sv(std::string_view s) {
    return std::hash<std::string_view>{}(s);
  }

  static constexpr const StringId kEmptyId = 0xFFFFFFFF;

  // bucket for open-addressing hash table
  struct Bucket {
    uint64_t hash = 0;
    StringId id = kEmptyId;
  };

  StringArena arena_;
  std::vector<Bucket> buckets_;
  double load_factor_ = 0.7;
};

}  // namespace base

#endif  // FRONTEND_BASE_STRING_STRING_INTERNER_H_
