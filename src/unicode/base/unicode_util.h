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
#include "unicode/base/unicode_export.h"

namespace unicode {

constexpr const char32_t kAsciiMax = 0x7F;

constexpr const char32_t kOneByteMin = 0;          // 0-127
constexpr const char32_t kTwoBytesMin = 0x80;      // 128-2047
constexpr const char32_t kThreeBytesMin = 0x800;   // 2048-65535
constexpr const char32_t kFourBytesMin = 0x10000;  // 65536-1114111

constexpr const char32_t kOneByteMax = kAsciiMax;   // 0-127
constexpr const char32_t kTwoBytesMax = 0x7FF;      // 128-2047
constexpr const char32_t kThreeBytesMax = 0xFFFF;   // 2048-65535
constexpr const char32_t kFourBytesMax = 0x10FFFF;  // 65536-1114111

inline constexpr bool is_in_ranges(const UnicodeRange* ranges,
                                   std::size_t count,
                                   char32_t codepoint) {
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

inline constexpr bool is_ascii(char32_t codepoint) {
  return codepoint <= kAsciiMax;
}

inline constexpr bool is_eof(char32_t codepoint) {
  return codepoint == 0;
}

inline constexpr bool is_ascii_letter(char32_t c) {
  // A-Z: 0x41-0x5A, a-z: 0x61-0x7A
  // the difference between uppercase and lowercase is only 0x20, so use bitwise
  // OR to normalize to lowercase
  constexpr const char32_t kDiff = 0x20;
  constexpr const char32_t kAlphabetsCount = 25;
  const char32_t normalized = c | kDiff;
  return (normalized - 'a') <= kAlphabetsCount;
}

// this will cause segmentation fault
// inline constexpr uint8_t utf8_sequence_length(unsigned char first_byte) {
//   return kUtf8LengthTable[first_byte];
// }

// implementation without utf8 length table
inline constexpr uint8_t utf8_sequence_length(uint8_t b) {
  if (b <= kOneByteMax) {
    return 1;
  } else if ((b & 0xE0) == 0xC0) {
    return 2;
  } else if ((b & 0xF0) == 0xE0) {
    return 3;
  } else if ((b & 0xF8) == 0xF0) {
    return 4;
  } else {
    return 0;
  }
}

inline constexpr uint8_t utf8_codepoint_length(char32_t c) {
  if (c <= kOneByteMax) {  // 0-127
    return 1;
  } else if (c <= kTwoBytesMax) {  // 128-2047
    return 2;
  } else if (c <= kThreeBytesMax) {  // 2048-65535
    return 3;
  } else if (c <= kFourBytesMax) {  // 65536-1114111
    return 4;
  }
  return 0;
}

inline bool is_valid_continuation_sequence(const uint8_t* bytes, uint8_t len) {
  // TODO: check all continuation bytes at once using simd where possible
  char32_t mask = 0;
  for (uint8_t i = 1; i < len; ++i) {
    mask |= (bytes[i] & 0xC0) ^ kTwoBytesMin;
  }
  return mask == 0;
}

inline constexpr bool is_valid_codepoint(char32_t cp, uint8_t len) {
  if (len < 1 || len > 4) [[unlikely]] {
    return false;
  }
  constexpr const char32_t kMinValues[5] = {
      0, kOneByteMin, kTwoBytesMin, kThreeBytesMin, kFourBytesMin,
  };
  constexpr const char32_t kMaxValues[5] = {
      0, kOneByteMax, kTwoBytesMax, kThreeBytesMax, kFourBytesMax,
  };
  return kMinValues[len] <= cp && cp <= kMaxValues[len] &&
         !(0xD800 <= cp && cp <= 0xDFFF);  // no surrogates
}

namespace detail {

inline constexpr bool is_ascii_digit(char32_t c) {
  return (c - '0') <= 9;
}

inline constexpr bool is_ascii_alnum(char32_t c) {
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

  constexpr bool operator[](char32_t c) const { return c < 128 && table[c]; }
};

inline constexpr bool generate_letter(int i) {
  return ('A' <= i && i <= 'Z') || ('a' <= i && i <= 'z');
}

inline constexpr bool generate_digit(int i) {
  return '0' <= i && i <= '9';
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
bool is_ascii_letters_bulk_avx2(const char32_t* codepoints,
                                bool* results,
                                std::size_t count);
bool is_ascii_digits_bulk_avx2(const char32_t* codepoints,
                               bool* results,
                               std::size_t count);
#endif  // ENABLE_AVX2

#if ENABLE_AVX2 && ENABLE_X86_ASM
extern "C" {
bool is_ascii_letters_bulk_avx2_x86(const char32_t* codepoints,
                                    bool* results,
                                    std::size_t count);
bool is_ascii_digits_bulk_avx2_x86(const char32_t* codepoints,
                                   bool* results,
                                   std::size_t count);
}
#endif  // ENABLE_AVX2 && ENABLE_X86_ASM

#if ENABLE_X86_ASM
extern "C" {
// in unicode_util.asm
bool is_ascii_letter_x86(char32_t codepoint);
bool is_ascii_digit_x86(char32_t codepoint);
bool is_ascii_alnum_x86(char32_t codepoint);

void is_ascii_letters_bulk_x86(const char32_t* codepoints,
                               bool* results,
                               std::size_t count);
void is_ascii_digits_bulk_x86(const char32_t* codepoints,
                              bool* results,
                              std::size_t count);

}  // extern "C"
#endif  // ENABLE_X86_ASM

}  // namespace detail

inline constexpr bool is_xid_start(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_letter_x86(codepoint) || codepoint == '_';
#else
    return detail::kAsciiIdStartTable[codepoint];
#endif
  }
  return is_in_ranges(kXIDStart, kXIDStartCount, codepoint);
}

inline constexpr bool is_xid_continue(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_alnum_x86(codepoint) || codepoint == '_';
#else
    return detail::kAsciiIdContinueTable[codepoint];
#endif
  }
  return is_in_ranges(kXIDContinue, kXIDContinueCount, codepoint);
}

inline constexpr bool is_unicode_whitespace(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // bitwise whitespace detection:
    // {' ', '\t', '\n', '\r', '\f', '\v'}
    // = {0x20, 0x09, 0x0A, 0x0D, 0x0C, 0x0B}
    constexpr const uint64_t whitespace_mask = (1ull << 0x20) | (1ull << 0x09) |
                                               (1ull << 0x0A) | (1ull << 0x0D) |
                                               (1ull << 0x0C) | (1ull << 0x0B);
    return codepoint <= 0x20 && ((whitespace_mask >> codepoint) & 1u);
  }
  return is_in_ranges(kWhiteSpace, kWhiteSpaceCount, codepoint);
}

inline constexpr bool is_unicode_newline(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // bitwise newline detection:
    // LF: 0x0A, VT: 0x0B, FF: 0x0C, CR: 0x0D
    constexpr const uint64_t ascii_newline_mask =
        (1ull << 0x0A) | (1ull << 0x0B) | (1ull << 0x0C) | (1ull << 0x0D);
    return codepoint <= 0x0D && (ascii_newline_mask & (1ull << codepoint));
  }

  // unicode newlines
  // NEL: 0x0085, LS: 0x2028, PS: 0x2029
  return codepoint == 0x0085 || codepoint == 0x2028 || codepoint == 0x2029;
}

inline constexpr bool is_decimal_number(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_digit_x86(codepoint);
#else
    return detail::kAsciiDigitTable[codepoint];
#endif
  }
  return is_in_ranges(kDecimalNumber, kDecimalNumberCount, codepoint);
}

inline constexpr bool is_uppercase_letter(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    return (codepoint - 'A') <= 25;
  }
  return is_in_ranges(kUppercaseLetter, kUppercaseLetterCount, codepoint);
}

inline constexpr bool is_lowercase_letter(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    return (codepoint - 'a') <= 25;
  }
  return is_in_ranges(kLowercaseLetter, kLowercaseLetterCount, codepoint);
}

inline constexpr bool is_titlecase_letter(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // ascii has no title case letters
    return false;
  }
  return is_in_ranges(kTitlecaseLetter, kTitlecaseLetterCount, codepoint);
}

inline constexpr bool is_modifier_letter(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // ascii has no modifier letters
    return false;
  }
  return is_in_ranges(kModifierLetter, kModifierLetterCount, codepoint);
}

inline constexpr bool is_other_letter(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
    // ascii has no other letters
    return false;
  }
  return is_in_ranges(kOtherLetter, kOtherLetterCount, codepoint);
}

inline constexpr bool is_letter(char32_t codepoint) {
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

inline constexpr bool is_alphanumeric(char32_t codepoint) {
  if (is_ascii(codepoint)) [[likely]] {
#if ENABLE_X86_ASM
    return detail::is_ascii_alnum_x86(codepoint);
#else
    return detail::kAsciiAlnumTable[codepoint];
#endif
  }
  return is_letter(codepoint) || is_decimal_number(codepoint);
}

#if ENABLE_AVX2 || ENABLE_X86_ASM
inline bool is_letters_bulk(const char32_t* codepoints,
                            bool* results,
                            std::size_t count) {
#if ENABLE_AVX2 && ENABLE_X86_ASM
  return detail::is_ascii_letters_bulk_avx2_x86(codepoints, results, count);
#elif ENABLE_AVX2
  return detail::is_ascii_letters_bulk_avx2(codepoints, results, count);
#elif ENABLE_X86_ASM
  return detail::is_ascii_letters_bulk_x86(codepoints, results, count);
#else
  static_assert(false);
#endif
}

inline bool is_digits_bulk(const char32_t* codepoints,
                           bool* results,
                           std::size_t count) {
#if ENABLE_AVX2 && ENABLE_X86_ASM
  return detail::is_ascii_digits_bulk_avx2_x86(codepoints, results, count);
#elif ENABLE_AVX2
  return detail::is_ascii_digits_bulk_avx2(codepoints, results, count);
#elif ENABLE_X86_ASM
  return detail::is_ascii_digits_bulk_x86(codepoints, results, count);
#else
  static_assert(false);
#endif
}
#endif  // ENABLE_AVX2

}  // namespace unicode

#endif  // UNICODE_BASE_UNICODE_UTIL_H_
