// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/base/char_stream.h"

#include <string>

#include "core/base/file_util.h"
#include "core/check.h"

namespace lexer {

CharStream::CharStream(FileManager* file_manager, FileId file_id)
    : file_manager_(file_manager), file_id_(file_id) {
  DCHECK(file_manager_);
}

char CharStream::peek() const {
  return pos_ < file().source().size() ? file().source()[pos_] : '\0';
}

char CharStream::peek_ahead(std::size_t n) const {
  return (pos_ + n < file().source().size()) ? file().source()[pos_ + n] : '\0';
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

}  // namespace lexer
