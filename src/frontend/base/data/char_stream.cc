// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/data/char_stream.h"

#include <string>

#include "core/base/file_util.h"
#include "core/base/string_util.h"
#include "core/check.h"

namespace base {

CharStream::CharStream(FileManager* file_manager, FileId file_id)
    : file_manager_(file_manager), file_id_(file_id) {
  DCHECK(file_manager_);
}

void CharStream::advance() {
  if (eof()) {
    return;
  }

  // for rewind()
  history_.push({pos_, line_, col_});

  if (file().source()[pos_] == '\n') {
    ++line_;
    col_ = 1;
  } else {
    ++col_;
  }

  ++pos_;
}

void CharStream::rewind() {
  if (!history_.empty()) {
    History h = history_.top();
    history_.pop();
    pos_ = h.pos;
    line_ = h.line;
    col_ = h.col;
  }
}

uint32_t CharStream::peek_codepoint() const {
  if (eof()) {
    return 0;
  }

  const unsigned char first_byte =
      static_cast<unsigned char>(file().source()[pos_]);
  const std::size_t len = core::utf8_codepoint_length(first_byte);

  if (len == 1) {
    return first_byte;
  }

  if (len == 2) {
    uint32_t codepoint = (first_byte & 0x1F) << 6;
    codepoint |= (file().source()[pos_ + 1] & 0x3F);
    return codepoint;
  }

  if (len == 3) {
    uint32_t codepoint = (first_byte & 0x0F) << 12;
    codepoint |= (file().source()[pos_ + 1] & 0x3F) << 6;
    codepoint |= (file().source()[pos_ + 2] & 0x3F);
    return codepoint;
  }

  if (len == 4) {
    uint32_t codepoint = (first_byte & 0x07) << 18;
    codepoint |= (file().source()[pos_ + 1] & 0x3F) << 12;
    codepoint |= (file().source()[pos_ + 2] & 0x3F) << 6;
    codepoint |= (file().source()[pos_ + 3] & 0x3F);
    return codepoint;
  }

  // invalid bytes sequence
  return 0;
}

void CharStream::advance_codepoint() {
  if (eof()) {
    return;
  }

  // save current state to history before advance
  history_.push({pos_, line_, col_});

  const unsigned char first_byte =
      static_cast<unsigned char>(file().source()[pos_]);
  const std::size_t len = core::utf8_codepoint_length(first_byte);

  pos_ += len;

  // FIXME: logic for updating line and column numbers in utf8.
  // currently, simply advancing the number of bytes using
  // `col_ += len;` is not sufficient for this part.
  // either perform the `\n` check that `advance()` does here as well, or modify
  // `advance_codepoint` to call `advance` multiple times.
  // example: `for (int i = 0; i < len; ++i) { advance(); }`.
}

}  // namespace base
