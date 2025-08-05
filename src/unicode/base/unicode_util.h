// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_BASE_UNICODE_UTIL_H_
#define UNICODE_BASE_UNICODE_UTIL_H_

#include <bit>
#include <cstddef>
#include <cstdint>

#if ENABLE_AVX2
#include <immintrin.h>
#endif

#include "unicode/base/ucd/unicode_data.h"

namespace unicode {

constexpr bool is_in_ranges(const UnicodeRange* ranges,
                            std::size_t count,
                            uint32_t codepoint) {
  if (count == 0) [[unlikely]] {
    return false;
  }

  if (codepoint < ranges[0].start || codepoint > ranges[count - 1].end)
      [[likely]] {
    return false;
  }

  std::size_t low = 0;
  std::size_t high = count;
  while (low < high) {
    std::size_t mid = low + ((high - low) >> 1);
    if (codepoint < ranges[mid].start) [[unlikely]] {
      high = mid;
    } else if (codepoint > ranges[mid].end) [[unlikely]] {
      low = mid + 1;
    } else [[likely]] {
      return true;
    }
  }
  return false;
}

inline constexpr bool is_ascii(uint32_t codepoint) {
  return codepoint <= 0x7F;
}

inline constexpr bool is_eof(uint32_t codepoint) {
  return codepoint == 0;
}

inline constexpr bool is_ascii_letter(uint32_t c) {
  // A-Z: 0x41-0x5A, a-z: 0x61-0x7A

  // the difference between uppercase and lowercase is only 0x20, so use bitwise
  // OR to normalize to lowercase
  uint32_t normalized = c | 0x20;
  return (normalized - 'a') <= 25;
}

namespace detail {

inline constexpr bool is_ascii_digit(uint32_t c) {
  return (c - '0') <= 9;
}

inline constexpr bool is_ascii_alnum(uint32_t c) {
  return is_ascii_letter(c) || is_ascii_digit(c);
}

template <bool (*Generator)(int)>
struct AsciiTable {
  bool table[128] = {};
  constexpr AsciiTable() {
    for (int i = 0; i < 128; ++i) {
      table[i] = Generator(i);
    }
  }

  constexpr bool operator[](uint32_t c) const { return c < 128 && table[c]; }
};

inline constexpr bool generate_letter(int i) {
  return (i >= 'A' && i <= 'Z') || (i >= 'a' && i <= 'z');
}

inline constexpr bool generate_digit(int i) {
  return i >= '0' && i <= '9';
}

inline constexpr bool generate_alnum(int i) {
  return generate_letter(i) || generate_digit(i);
}

inline constexpr bool generate_id_start(int i) {
  return generate_letter(i) || i == '_';
}

inline constexpr bool generate_id_continue(int i) {
  return generate_alnum(i) || i == '_';
}

constexpr auto kAsciiLetterTable = AsciiTable<generate_letter>{};
constexpr auto kAsciiDigitTable = AsciiTable<generate_digit>{};
constexpr auto kAsciiAlnumTable = AsciiTable<generate_alnum>{};
constexpr auto kAsciiIdStartTable = AsciiTable<generate_id_start>{};
constexpr auto kAsciiIdContinueTable = AsciiTable<generate_id_continue>{};

#if ENABLE_AVX2
bool is_ascii_letters_bulk_avx2(const uint32_t* codepoints,
                                bool* results,
                                std::size_t count);
bool is_ascii_digits_bulk_avx2(const uint32_t* codepoints,
                               bool* results,
                               std::size_t count);
#endif  // ENABLE_AVX2

#if ENABLE_X86_ASM
extern "C" {
// in unicode_util.asm
bool is_ascii_letter_x86(uint32_t codepoint);
bool is_ascii_digit_x86(uint32_t codepoint);
bool is_ascii_alnum_x86(uint32_t codepoint);

void is_ascii_letters_bulk_x86(const uint32_t* codepoints,
                               bool* results,
                               std::size_t count);
void is_ascii_digits_bulk_x86(const uint32_t* codepoints,
                              bool* results,
                              std::size_t count);

}  // extern "C"
#endif  // ENABLE_X86_ASM

}  // namespace detail

inline constexpr bool is_xid_start(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_letter_x86(codepoint) || codepoint == '_';
#else
    return detail::kAsciiIdStartTable[codepoint];
#endif
  }
  return is_in_ranges(kXIDStart, kXIDStartCount, codepoint);
}

inline constexpr bool is_xid_continue(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_alnum_x86(codepoint) || codepoint == '_';
#else
    return detail::kAsciiIdContinueTable[codepoint];
#endif
  }
  return is_in_ranges(kXIDContinue, kXIDContinueCount, codepoint);
}

inline constexpr bool is_unicode_whitespace(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // bitwise whitespace detection:
    // {' ', '\t', '\n', '\r', '\f', '\v'}
    // = {0x20, 0x09, 0x0A, 0x0D, 0x0C, 0x0B}
    constexpr uint64_t whitespace_mask = (1ull << 0x20) | (1ull << 0x09) |
                                         (1ull << 0x0A) | (1ull << 0x0D) |
                                         (1ull << 0x0C) | (1ull << 0x0B);
    return codepoint <= 0x20 && (whitespace_mask & (1ull << codepoint));
  }
  return is_in_ranges(kWhiteSpace, kWhiteSpaceCount, codepoint);
}

inline constexpr bool is_decimal_number(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_digit_x86(codepoint);
#else
    return detail::kAsciiDigitTable[codepoint];
#endif
  }
  return is_in_ranges(kDecimalNumber, kDecimalNumberCount, codepoint);
}

inline constexpr bool is_uppercase_letter(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    return (codepoint - 'A') <= 25;
  }
  return is_in_ranges(kUppercaseLetter, kUppercaseLetterCount, codepoint);
}

inline constexpr bool is_lowercase_letter(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    return (codepoint - 'a') <= 25;
  }
  return is_in_ranges(kLowercaseLetter, kLowercaseLetterCount, codepoint);
}

inline constexpr bool is_titlecase_letter(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // ascii has no title case letters
    return false;
  }
  return is_in_ranges(kTitlecaseLetter, kTitlecaseLetterCount, codepoint);
}

inline constexpr bool is_modifier_letter(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // ascii has no modifier letters
    return false;
  }
  return is_in_ranges(kModifierLetter, kModifierLetterCount, codepoint);
}

inline constexpr bool is_other_letter(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // ascii has no other letters
    return false;
  }
  return is_in_ranges(kOtherLetter, kOtherLetterCount, codepoint);
}

inline constexpr bool is_letter(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_letter_x86(codepoint);
#else
    return detail::kAsciiLetterTable[codepoint];
#endif
  }
  // non-ascii range multiple checks
  return is_uppercase_letter(codepoint) || is_lowercase_letter(codepoint) ||
         is_titlecase_letter(codepoint) || is_modifier_letter(codepoint) ||
         is_other_letter(codepoint);
}

inline constexpr bool is_alphanumeric(uint32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_alnum_x86(codepoint);
#else
    return detail::kAsciiAlnumTable[codepoint];
#endif
  }
  return is_letter(codepoint) || is_decimal_number(codepoint);
}

#if ENABLE_AVX2
inline bool is_letters_bulk(const uint32_t* codepoints,
                            bool* results,
                            std::size_t count) {
  return detail::is_ascii_letters_bulk_avx2(codepoints, results, count);
}

inline bool is_digits_bulk(const uint32_t* codepoints,
                           bool* results,
                           std::size_t count) {
  return detail::is_ascii_digits_bulk_avx2(codepoints, results, count);
}
#endif  // ENABLE_AVX2

}  // namespace unicode

#endif  // UNICODE_BASE_UNICODE_UTIL_H_
