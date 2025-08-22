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

  Utf8FileManager(Utf8FileManager&&) noexcept = default;
  Utf8FileManager& operator=(Utf8FileManager&&) noexcept = default;

  Utf8FileId register_file(std::u8string_view file_name);
  Utf8FileId register_file_loaded(std::u8string_view file_name,
                                  std::u8string&& source);
  Utf8FileId register_virtual_file(std::u8string&& source);

  inline void load(Utf8FileId id) { file_mutable(id).load(); }
  inline void unload(Utf8FileId id) { file_mutable(id).unload(); }

  inline const Utf8File& file(Utf8FileId id) const {
    DCHECK_GE(id, 0);
    DCHECK_LE(id, files_.size());
    return files_[id];
  }

  inline const Utf8File& loaded_file(Utf8FileId id) {
    auto& f = file_mutable(id);
    if (!f.loaded()) {
      f.load();
    }
    return f;
  }

 private:
  inline Utf8File& file_mutable(Utf8FileId id) {
    DCHECK_GE(id, 0);
    DCHECK_LE(id, files_.size());
    return files_[id];
  }

  std::vector<Utf8File> files_;
};

}  // namespace unicode

#endif  // UNICODE_UTF8_FILE_MANAGER_H_
