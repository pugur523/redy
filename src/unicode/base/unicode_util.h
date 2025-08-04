// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_BASE_UNICODE_UTIL_H_
#define UNICODE_BASE_UNICODE_UTIL_H_

#include <cstddef>
#include <cstdint>

#include "unicode/base/ucd/unicode_data.h"

namespace unicode {

constexpr bool is_in_ranges(const UnicodeRange* ranges,
                            std::size_t count,
                            uint32_t codepoint) {
  std::size_t low = 0;
  std::size_t high = count;
  while (low < high) {
    std::size_t mid = (low + high) / 2;
    if (codepoint < ranges[mid].start) {
      high = mid;
    } else if (codepoint > ranges[mid].end) {
      low = mid + 1;
    } else {
      return true;
    }
  }
  return false;
}

// XID functions
inline constexpr bool is_xid_start(uint32_t codepoint) {
  return is_in_ranges(kXIDStart, kXIDStartCount, codepoint);
}

inline constexpr bool is_xid_continue(uint32_t codepoint) {
  return is_in_ranges(kXIDContinue, kXIDContinueCount, codepoint);
}

// Whitespace function
inline constexpr bool is_unicode_whitespace(uint32_t codepoint) {
  return is_in_ranges(kWhiteSpace, kWhiteSpaceCount, codepoint);
}

// General category functions
inline constexpr bool is_decimal_number(uint32_t codepoint) {
  return is_in_ranges(kDecimalNumber, kDecimalNumberCount, codepoint);
}

inline constexpr bool is_uppercase_letter(uint32_t codepoint) {
  return is_in_ranges(kUppercaseLetter, kUppercaseLetterCount, codepoint);
}

inline constexpr bool is_lowercase_letter(uint32_t codepoint) {
  return is_in_ranges(kLowercaseLetter, kLowercaseLetterCount, codepoint);
}

inline constexpr bool is_titlecase_letter(uint32_t codepoint) {
  return is_in_ranges(kTitlecaseLetter, kTitlecaseLetterCount, codepoint);
}

inline constexpr bool is_modifier_letter(uint32_t codepoint) {
  return is_in_ranges(kModifierLetter, kModifierLetterCount, codepoint);
}

inline constexpr bool is_other_letter(uint32_t codepoint) {
  return is_in_ranges(kOtherLetter, kOtherLetterCount, codepoint);
}

inline constexpr bool is_ascii(uint32_t codepoint) {
  return codepoint <= 0x7F;
}

inline constexpr bool is_eof(uint32_t codepoint) {
  return codepoint == 0;
}

// Composite functions
inline constexpr bool is_letter(uint32_t codepoint) {
  return is_uppercase_letter(codepoint) || is_lowercase_letter(codepoint) ||
         is_titlecase_letter(codepoint) || is_modifier_letter(codepoint) ||
         is_other_letter(codepoint);
}

inline constexpr bool is_alphanumeric(uint32_t codepoint) {
  return is_letter(codepoint) || is_decimal_number(codepoint);
}

// Common identifier character check (more comprehensive than XID)
inline constexpr bool is_identifier_start(uint32_t codepoint) {
  return is_letter(codepoint) || codepoint == '_';
}

inline constexpr bool is_identifier_continue(uint32_t codepoint) {
  return is_alphanumeric(codepoint) || codepoint == '_';
}

}  // namespace unicode

#endif  // UNICODE_BASE_UNICODE_UTIL_H_
