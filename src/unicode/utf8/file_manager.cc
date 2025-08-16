// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "unicode/utf8/file_manager.h"

#include <string>
#include <utility>

#include "core/base/file_util.h"

namespace unicode {

Utf8FileId Utf8FileManager::register_file(std::u8string_view file_name) {
  DCHECK(!file_name.empty())
      << "file name is empty. use `add_virtual_file` for testing purposes.";

  files_.emplace_back();
  files_.back().init(file_name);
  return files_.size() - 1;
}

Utf8FileId Utf8FileManager::register_file_loaded(std::u8string_view file_name,
                                                 std::u8string&& source) {
  DCHECK(!file_name.empty())
      << "file name is empty. use `add_virtual_file` for testing purposes.";

  files_.emplace_back();
  files_.back().init_loaded(file_name, std::move(source));
  return files_.size() - 1;
}

Utf8FileId Utf8FileManager::register_virtual_file(std::u8string&& source) {
  static std::size_t virtual_file_count = 0;
  constexpr std::string_view kPrefix = "virtual_file_";
  constexpr std::string_view kExtension = ".ry";

  std::string virtual_name;
  virtual_name.append(kPrefix);
  virtual_name.append(std::to_string(virtual_file_count++));
  virtual_name.append(kExtension);
  std::u8string_view virtual_name_u8(
      reinterpret_cast<const char8_t*>(virtual_name.c_str()),
      virtual_name.size());

  return register_file_loaded(virtual_name_u8, std::move(source));
}

}  // namespace unicode
