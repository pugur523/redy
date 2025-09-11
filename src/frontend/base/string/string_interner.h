// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_STRING_STRING_INTERNER_H_
#define FRONTEND_BASE_STRING_STRING_INTERNER_H_

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

#include "frontend/base/base_export.h"
#include "frontend/base/data/string_arena.h"
#include "frontend/base/string/string_id.h"

namespace base {

constexpr const StringId kEmptyStringId = 0;
constexpr const StringId kInvalidStringId = 0xFFFFFFFF;

class BASE_EXPORT StringInterner {
 public:
  explicit StringInterner(double max_load_factor = 0.75);
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

  inline std::size_t string_count() const { return arena_.size(); }
  inline std::size_t bucket_count() const { return buckets_.size(); }
  inline std::size_t used_buckets() const { return used_buckets_; }
  inline double load_factor() const {
    return buckets_.empty() ? 0.0
                            : static_cast<double>(used_buckets_) /
                                  static_cast<double>(buckets_.size());
  }

 private:
  void init_buckets(std::size_t n);
  void ensure_load_factor();
  void rehash(std::size_t new_size);

  // TODO: replace this with xxh3
  static inline uint64_t hash_sv(std::string_view s) {
    // FNV-1a hash (better than std::hash for short strings)
    // FNV offset basis
    uint64_t hash = 14695981039346656037ULL;
    for (char c : s) {
      hash ^= static_cast<uint64_t>(c);
      // FNV prime
      hash *= 1099511628211ULL;
    }
    return hash;
  }

  // bucket for open-addressing hash table
  struct Bucket {
    uint64_t hash = 0;
    StringId id = kInvalidStringId;

    bool empty() const { return id == kInvalidStringId; }
  };

  StringArena arena_;
  std::vector<Bucket> buckets_;
  std::size_t used_buckets_ = 0;
  double max_load_factor_;
};

}  // namespace base

#endif  // FRONTEND_BASE_STRING_STRING_INTERNER_H_
