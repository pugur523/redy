// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef UNICODE_BASE_UCD_UNICODE_DATA_H_
#define UNICODE_BASE_UCD_UNICODE_DATA_H_

#include <cstddef>
#include <cstdint>

namespace unicode {

struct UnicodeRange {
  uint32_t start;
  uint32_t end;
};

// XID (eXtended IDentifier) properties
extern const UnicodeRange kXIDStart[];
extern const std::size_t kXIDStartCount;
extern const UnicodeRange kXIDContinue[];
extern const std::size_t kXIDContinueCount;

// Whitespace property
extern const UnicodeRange kWhiteSpace[];
extern const std::size_t kWhiteSpaceCount;

// General categories
extern const UnicodeRange kDecimalNumber[];  // Nd
extern const std::size_t kDecimalNumberCount;
extern const UnicodeRange kUppercaseLetter[];  // Lu
extern const std::size_t kUppercaseLetterCount;
extern const UnicodeRange kLowercaseLetter[];  // Ll
extern const std::size_t kLowercaseLetterCount;
extern const UnicodeRange kTitlecaseLetter[];  // Lt
extern const std::size_t kTitlecaseLetterCount;
extern const UnicodeRange kModifierLetter[];  // Lm
extern const std::size_t kModifierLetterCount;
extern const UnicodeRange kOtherLetter[];  // Lo
extern const std::size_t kOtherLetterCount;

constexpr const uint8_t kUtf8LengthTable[256] = {
    // 0x00-0x7f: 1 byte
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1,
    // 0x80-0xbf: invalid start bytes
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // 0xc0-0xdf: 2 bytes (0xc0-0xc1 are overlong)
    0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2,
    // 0xe0-0xef: 3 bytes
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    // 0xf0-0xf7: 4 bytes, 0xf8-0xff: invalid
    4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0};

}  // namespace unicode

#endif  // UNICODE_BASE_UCD_UNICODE_DATA_H_
