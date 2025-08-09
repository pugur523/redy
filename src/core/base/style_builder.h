// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef CORE_BASE_STYLE_BUILDER_H_
#define CORE_BASE_STYLE_BUILDER_H_

#include <string>
#include <string_view>

#include "core/base/core_export.h"
#include "core/base/style_util.h"

namespace core {

extern const bool can_use_ansi_sequence;

class CORE_EXPORT StyleBuilder {
 public:
  constexpr StyleBuilder() = default;

  ~StyleBuilder() = default;

  StyleBuilder(const StyleBuilder&) = delete;
  StyleBuilder& operator=(const StyleBuilder&) = delete;

  StyleBuilder(StyleBuilder&&) = default;
  StyleBuilder& operator=(StyleBuilder&&) = default;

  inline constexpr StyleBuilder& style(Style s) {
    styles_ = s;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& add_style(Style s) {
    styles_ |= s;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& remove_style(Style s) {
    styles_ &= ~s;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& clear_style() {
    styles_ = Style::kDefault;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& colour(Colour c) {
    fg_ = c;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& bg(BgColour c) {
    bg_ = c;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& rgb(uint8_t r, uint8_t g, uint8_t b) {
    fg_rgb_[0] = r;
    fg_rgb_[1] = g;
    fg_rgb_[2] = b;
    use_fg_rgb_ = true;
    update_should_build_cache();
    return *this;
  }

  inline constexpr StyleBuilder& bg_rgb(uint8_t r, uint8_t g, uint8_t b) {
    bg_rgb_[0] = r;
    bg_rgb_[1] = g;
    bg_rgb_[2] = b;
    use_bg_rgb_ = true;
    update_should_build_cache();
    return *this;
  }

  std::string build(std::string_view text) const;

  inline std::string build(const char* ptr, std::size_t size) const {
    return build(std::string_view(ptr, size));
  }

  inline std::string build(std::string&& text) const {
    // zero copy
    if (!should_build_) {
      return text;
    }
    return build(std::string_view(text));
  }

  inline std::string build(const char* text) {
    return build(std::string_view(text));
  }

  inline constexpr void reset() {
    this->~StyleBuilder();
    *this = StyleBuilder();
  }

 private:
  inline constexpr void update_should_build_cache() {
    if (!can_use_ansi_sequence) {
      should_build_ = false;
      return;
    }

    if (styles_ != Style::kDefault) {
      should_build_ = true;
      return;
    }

    if (use_fg_rgb_) {
      should_build_ = true;
      return;
    } else if (fg_ != Colour::kDefault) {
      should_build_ = true;
      return;
    }

    if (use_bg_rgb_) {
      should_build_ = true;
      return;
    } else if (bg_ != BgColour::kDefault) {
      should_build_ = false;
      return;
    }
  }

  Style styles_ = Style::kDefault;

  Colour fg_ = Colour::kDefault;
  BgColour bg_ = BgColour::kDefault;

  bool should_build_ : 1 = false;
  bool use_fg_rgb_ : 1 = false;
  bool use_bg_rgb_ : 1 = false;
  uint8_t fg_rgb_[3];
  uint8_t bg_rgb_[3];
};

}  // namespace core

#endif  // CORE_BASE_STYLE_BUILDER_H_
