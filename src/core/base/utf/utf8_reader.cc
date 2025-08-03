// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "core/base/utf/utf8_reader.h"

#include <vector>

namespace core {

Utf8Reader::Utf8Reader(const std::vector<char>& utf8) : utf8_(utf8) {}

Utf8Reader::Iterator Utf8Reader::begin() const {
  return Iterator(utf8_.data(), utf8_.data() + utf8_.size());
}

Utf8Reader::Iterator Utf8Reader::end() const {
  return Iterator(utf8_.data() + utf8_.size(), utf8_.data() + utf8_.size());
}

Utf8Reader::Iterator::Iterator(const char* ptr, const char* end)
    : ptr_(ptr), end_(end), current_(0) {
  if (ptr_ < end_) {
    decode();
  }
}

char32_t Utf8Reader::Iterator::operator*() const {
  return current_;
}

bool Utf8Reader::Iterator::operator!=(const Iterator& other) const {
  return ptr_ != other.ptr_;
}

Utf8Reader::Iterator& Utf8Reader::Iterator::operator++() {
  if (ptr_ >= end_) {
    return *this;
  }

  unsigned char lead = static_cast<unsigned char>(*ptr_);
  std::size_t len = 0;

  if (lead < 0x80) {
    len = 1;
  } else if ((lead >> 5) == 0x6) {
    len = 2;
  } else if ((lead >> 4) == 0xE) {
    len = 3;
  } else if ((lead >> 3) == 0x1E) {
    len = 4;
  } else {
    // invalid
    len = 1;
  }

  ptr_ += len;
  if (ptr_ < end_) {
    decode();
  }
  return *this;
}

void Utf8Reader::Iterator::decode() const {
  if (ptr_ >= end_) {
    current_ = 0xFFFD;
    return;
  }

  const unsigned char* p = reinterpret_cast<const unsigned char*>(ptr_);
  std::size_t remaining = end_ - ptr_;
  uint32_t codepoint = 0;

  if (p[0] < 0x80) {
    current_ = p[0];
  } else if ((p[0] >> 5) == 0x6 && remaining >= 2) {
    codepoint = ((p[0] & 0x1F) << 6) | (p[1] & 0x3F);
    current_ = codepoint;
  } else if ((p[0] >> 4) == 0xE && remaining >= 3) {
    codepoint = ((p[0] & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
    current_ = codepoint;
  } else if ((p[0] >> 3) == 0x1E && remaining >= 4) {
    codepoint = ((p[0] & 0x07) << 18) | ((p[1] & 0x3F) << 12) |
                ((p[2] & 0x3F) << 6) | (p[3] & 0x3F);
    current_ = codepoint;
  } else {
    current_ = 0xFFFD;  // invalid sequence
  }
}

}  // namespace core
