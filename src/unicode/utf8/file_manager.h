// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_UTF8_FILE_MANAGER_H_
#define UNICODE_UTF8_FILE_MANAGER_H_

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "unicode/base/unicode_export.h"
#include "unicode/utf8/file.h"

namespace unicode {

using Utf8FileId = uint32_t;

inline constexpr const Utf8FileId kInvalidFileId =
    std::numeric_limits<Utf8FileId>::max();

class UNICODE_EXPORT Utf8FileManager {
 public:
  Utf8FileManager() = default;

  ~Utf8FileManager() = default;

  Utf8FileManager(const Utf8FileManager&) = delete;
  Utf8FileManager& operator=(const Utf8FileManager&) = delete;

  Utf8FileManager(Utf8FileManager&&) = default;
  Utf8FileManager& operator=(Utf8FileManager&&) = default;

  Utf8FileId add_file(std::u8string&& source, std::u8string&& file_name);
  Utf8FileId add_file(std::u8string&& file_name);
  Utf8FileId add_virtual_file(std::u8string&& source);

  const Utf8File& file(Utf8FileId id) const;

 private:
  std::vector<Utf8File> files_;
};

}  // namespace unicode

#endif  // UNICODE_UTF8_FILE_MANAGER_H_
