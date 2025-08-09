// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/file.h"

#include <utility>

#include "core/base/file_util.h"
#include "core/base/string_util.h"
#include "core/check.h"
#include "frontend/diagnostic/data/diagnostic_id.h"

namespace unicode {

void Utf8File::init(std::u8string&& file_name, std::u8string&& content) {
  DCHECK_EQ(status_, Status::kNotInitialized);

  file_name_ = std::move(file_name);
  content_ = std::move(content);
  line_ends_ = core::index_newlines(core::to_string_view(content_));
  status_ = Status::kInitialized;
}

void Utf8File::init(std::u8string&& file_name) {
  DCHECK_EQ(status_, Status::kNotInitialized);
  std::u8string content = core::read_file_utf8(file_name.c_str());
  init(std::move(file_name), std::move(content));
}

}  // namespace unicode
