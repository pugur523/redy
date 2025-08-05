// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/data/char_stream.h"

#include <string>
#include <utility>

#include "core/base/file_util.h"
#include "core/base/string_util.h"
#include "core/check.h"

namespace base {

CharStream::CharStream(const File* file) : file_(file) {
  DCHECK(file_);
}

void CharStream::advance() {
  if (eof()) {
    return;
  }
  rewind_buffer_[(rewind_index_ + rewind_size_) % kRewindHistorySize] = cursor_;
  if (rewind_size_ < kRewindHistorySize) {
    ++rewind_size_;
  } else {
    rewind_index_ = (rewind_index_ + 1) % kRewindHistorySize;
  }

  if (file().source()[cursor_.pos] == '\n') {
    ++cursor_.line;
    cursor_.col = 1;
    cursor_.cp_col = 1;
  } else {
    ++cursor_.col;
    ++cursor_.cp_col;
  }

  ++cursor_.pos;
}

void CharStream::rewind() {
  if (rewind_size_ == 0) {
    return;
  }
  rewind_index_ = (rewind_index_ + kRewindHistorySize - 1) % kRewindHistorySize;
  cursor_ = rewind_buffer_[rewind_index_];
  --rewind_size_;
}

uint32_t CharStream::peek_codepoint(std::size_t n) const {
  const std::string& source = file().source();
  std::size_t max_size = source.size();
  std::size_t current_pos = cursor_.pos;

  if (n == 0) {
    if (current_pos != cached_pos_ || !current_codepoint_cache_) {
      auto [codepoint, _] =
          decode_utf8_at(source.data(), current_pos, max_size);
      current_codepoint_cache_ = codepoint;
      cached_pos_ = current_pos;
    }
    return *current_codepoint_cache_;
  }

  for (std::size_t i = 0; i < n && current_pos < max_size; ++i) {
    auto [cp, len] = decode_utf8_at(source.data(), current_pos, max_size);
    if (cp == 0) {
      return 0;
    }
    current_pos += len;
  }

  if (current_pos >= max_size) {
    return 0;  // eof
  }

  auto [cp, _] = decode_utf8_at(source.data(), current_pos, max_size);
  return cp;
}

void CharStream::advance_codepoint() {
  if (eof()) {
    return;
  }

  rewind_buffer_[(rewind_index_ + rewind_size_) % kRewindHistorySize] = cursor_;
  if (rewind_size_ < kRewindHistorySize) {
    ++rewind_size_;
  } else {
    rewind_index_ = (rewind_index_ + 1) % kRewindHistorySize;
  }

  const std::string& source = file().source();
  auto [codepoint, byte_length] =
      decode_utf8_at(source.data(), cursor_.pos, source.size());
  update_position_for_codepoint(codepoint, byte_length);
  cursor_.pos += byte_length;
  cached_pos_ = cursor_.pos;
  current_codepoint_cache_ = std::nullopt;
}

void CharStream::advance_codepoint_fast() {
  if (eof()) {
    return;
  }
  const std::string& source = file().source();
  unsigned char first = source[cursor_.pos];
  std::size_t len = utf8_sequence_length(first);
  cursor_.pos += len;
}

std::size_t CharStream::codepoint_byte_length() const {
  if (eof()) {
    return 0;
  }
  return utf8_sequence_length(
      static_cast<unsigned char>(file().source()[cursor_.pos]));
}

bool CharStream::is_valid_utf8_current_codepoint() const {
  if (eof()) {
    return true;
  }

  const std::string& source = file().source();
  auto [codepoint, _] =
      decode_utf8_at(source.data(), cursor_.pos, source.size());
  return codepoint != kInvalidUtfChar;
}

bool CharStream::is_valid_utf8_whole_stream() const {
  const std::string& source = file().source();
  std::size_t i = 0;
  while (i < source.size()) {
    auto [cp, len] = decode_utf8_at(source.data(), i, source.size());
    if (cp == kInvalidUtfChar) {
      return false;
    }
    i += len;
  }
  return true;
}

constexpr std::pair<uint32_t, std::size_t> CharStream::decode_utf8_at(
    const char* data,
    std::size_t pos,
    std::size_t max_size) noexcept {
  if (pos >= max_size) {
    return {0, 1};
  }
  const unsigned char* bytes =
      reinterpret_cast<const unsigned char*>(data + pos);
  std::size_t remaining = max_size - pos;
  unsigned char first = bytes[0];

  if (first < 0x80) {
    return {first, 1};
  }
  if ((first >> 5) == 0x06 && remaining >= 2 && (bytes[1] & 0xC0) == 0x80) {
    uint32_t cp = ((first & 0x1F) << 6) | (bytes[1] & 0x3F);
    return cp >= 0x80
               ? std::make_pair(cp, 2)
               : std::make_pair(static_cast<uint32_t>(kInvalidUtfChar), 2);
  }
  if ((first >> 4) == 0x0E && remaining >= 3 && (bytes[1] & 0xC0) == 0x80 &&
      (bytes[2] & 0xC0) == 0x80) {
    uint32_t cp =
        ((first & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
    return (cp >= 0x800 && !(cp >= 0xD800 && cp <= 0xDFFF))
               ? std::make_pair(cp, 3)
               : std::make_pair(kInvalidUtfChar, 3);
  }
  if ((first >> 3) == 0x1E && remaining >= 4 && (bytes[1] & 0xC0) == 0x80 &&
      (bytes[2] & 0xC0) == 0x80 && (bytes[3] & 0xC0) == 0x80) {
    uint32_t cp = ((first & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) |
                  ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
    return (cp >= 0x10000 && cp <= 0x10FFFF)
               ? std::make_pair(cp, 4)
               : std::make_pair(kInvalidUtfChar, 4);
  }
  return {kInvalidUtfChar, 1};
}

void CharStream::update_position_for_codepoint(uint32_t codepoint,
                                               std::size_t byte_length) {
  if (codepoint == '\n') {
    ++cursor_.line;
    cursor_.col = 1;
    cursor_.cp_col = 1;
  } else if (codepoint == '\t') {
    cursor_.col += byte_length;
    std::size_t tab_stop = ((cursor_.cp_col - 1) / 8 + 1) * 8;
    cursor_.cp_col = tab_stop + 1;
  } else {
    cursor_.col += byte_length;
    ++cursor_.cp_col;
  }
}

}  // namespace base
