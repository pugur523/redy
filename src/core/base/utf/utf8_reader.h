// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_UTF_UTF8_READER_H_
#define CORE_BASE_UTF_UTF8_READER_H_

#include <cstdint>
#include <iterator>
#include <vector>

#include "core/base/core_export.h"

namespace core {

class CORE_EXPORT Utf8Reader {
 public:
  class Iterator {
   public:
    using iterator_category = std::input_iterator_tag;
    using value_type = char32_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const char32_t*;
    using reference = char32_t;

    Iterator(const char* ptr, const char* end);
    char32_t operator*() const;
    Iterator& operator++();
    bool operator!=(const Iterator& other) const;

   private:
    const char* ptr_;
    const char* end_;
    mutable char32_t current_;
    void decode() const;
  };

  // explicit Utf8Reader(const std::string& utf8);
  explicit Utf8Reader(const std::vector<char>& utf8);

  Utf8Reader() = delete;
  ~Utf8Reader() = delete;

  Utf8Reader(const Utf8Reader&) = delete;
  Utf8Reader& operator=(const Utf8Reader&) = delete;
  Utf8Reader& operator=(Utf8Reader&&) = delete;

  Utf8Reader(Utf8Reader&&) = default;

  Iterator begin() const;
  Iterator end() const;

 private:
  const std::vector<char>& utf8_;
};

}  // namespace core

#endif  // CORE_BASE_UTF_UTF8_READER_H_
