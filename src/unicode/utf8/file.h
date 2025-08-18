// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_UTF8_FILE_H_
#define UNICODE_UTF8_FILE_H_

#include <string>
#include <string_view>
#include <vector>

#include "core/check.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "unicode/base/unicode_export.h"

namespace unicode {

class UNICODE_EXPORT Utf8File {
 public:
  enum class Status : uint8_t {
    kNotInitialized = 0,

    kNotLoaded = 1,
    kLoaded = 2,
  };

  Utf8File() = default;
  ~Utf8File() = default;

  Utf8File(const Utf8File&) = delete;
  Utf8File& operator=(const Utf8File&) = delete;

  Utf8File(Utf8File&&) = default;
  Utf8File& operator=(Utf8File&&) = default;

  void init(std::u8string_view file_name);
  void init_and_load(std::u8string_view file_name, std::u8string&& content);

  void load();
  void unload();

  inline bool loaded() const { return status_ == Status::kLoaded; }

  inline std::u8string_view file_name_u8() const {
    DCHECK_EQ(status_, Status::kLoaded);
    return file_name_;
  }

  inline std::u8string_view content_u8() const {
    DCHECK_EQ(status_, Status::kLoaded);
    return content_;
  }

  // 1 indexed
  inline std::u8string_view line_u8(std::size_t line_no) const {
    DCHECK_EQ(status_, Status::kLoaded);
    DCHECK_GT(line_no, 0);
    DCHECK_LE(line_no, line_count());

    std::size_t line_start = (line_no == 1) ? 0 : line_ends_[line_no - 2] + 1;
    std::size_t line_end = line_ends_[line_no - 1];

    // crlf
    if (line_end > line_start && content_[line_end - 1] == '\r') {
      --line_end;
    }

    return std::u8string_view(&content_[line_start], line_end - line_start);
  }

  inline std::string_view file_name() const {
    return core::to_string_view(file_name_u8());
  }

  inline std::string_view content() const {
    return core::to_string_view(content_u8());
  }

  inline std::string_view line(std::size_t line_no) const {
    return core::to_string_view(line_u8(line_no));
  }

  inline std::size_t line_count() const {
    DCHECK_EQ(status_, Status::kLoaded);
    return line_ends_.size();
  }

  inline std::string_view slice(std::size_t pos, std::size_t len) const {
    DCHECK_EQ(status_, Status::kLoaded);
    return std::string_view(reinterpret_cast<const char*>(&content_[pos]), len);
  }

 private:
  std::u8string file_name_ = u8"";
  std::u8string content_ = u8"";
  std::vector<std::size_t> line_ends_;

  Status status_ = Status::kNotInitialized;
};

}  // namespace unicode

#endif  // UNICODE_UTF8_FILE_H_
