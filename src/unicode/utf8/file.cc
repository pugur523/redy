// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/file.h"

#include <string>
#include <utility>
#include <vector>

#include "core/base/file_util.h"
#include "core/check.h"
#include "frontend/diagnostic/data/diagnostic_id.h"

namespace unicode {

void Utf8File::init(std::u8string_view file_name) {
  DCHECK_EQ(status_, Status::kNotInitialized);
  file_name_ = file_name;
  status_ = Status::kNotLoaded;
}

void Utf8File::init_and_load(std::u8string_view file_name,
                             std::u8string&& content) {
  DCHECK_EQ(status_, Status::kNotInitialized);
  init(file_name);
  content_ = std::move(content);
  line_ends_ = core::index_newlines(core::to_string_view(content_));
  status_ = Status::kLoaded;
}

void Utf8File::load() {
  DCHECK_EQ(status_, Status::kNotLoaded);
  content_ = core::read_file_utf8(file_name_.c_str());
  line_ends_ = core::index_newlines(core::to_string_view(content_));
  status_ = Status::kLoaded;
}

void Utf8File::unload() {
  DCHECK_EQ(status_, Status::kLoaded);
  content_ = std::u8string();
  line_ends_ = std::vector<std::size_t>();
  status_ = Status::kNotLoaded;
}

}  // namespace unicode
