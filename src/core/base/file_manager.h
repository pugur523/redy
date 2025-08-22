// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_FILE_MANAGER_H_
#define CORE_BASE_FILE_MANAGER_H_

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "core/base/core_export.h"
#include "core/base/file_util.h"

namespace core {

using FileId = uint32_t;
using RawFileId = uint32_t;

inline constexpr const FileId kInvalidFileId =
    std::numeric_limits<FileId>::max();

class CORE_EXPORT FileManager {
 public:
  FileManager() = default;

  ~FileManager() = default;

  FileManager(const FileManager&) = delete;
  FileManager& operator=(const FileManager&) = delete;

  FileManager(FileManager&&) noexcept = default;
  FileManager& operator=(FileManager&&) noexcept = default;

  [[nodiscard]] FileId add_file(std::string&& source, std::string&& file_name);
  [[nodiscard]] FileId add_file(std::string&& file_name);
  [[nodiscard]] FileId add_virtual_file(std::string&& source);

  [[nodiscard]] const File& file(FileId id) const;

 private:
  std::vector<File> files_;
};

}  // namespace core

#endif  // CORE_BASE_FILE_MANAGER_H_
