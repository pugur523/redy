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

}  // namespace unicode

#endif  // UNICODE_BASE_UCD_UNICODE_DATA_H_
