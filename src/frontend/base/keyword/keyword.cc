// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/base/keyword/keyword.h"

#include <array>
#include <cstring>
#include <string>
#include <string_view>

#include "frontend/base/token/token.h"
#include "frontend/base/token/token_kind.h"

namespace base {

namespace {

constexpr std::size_t calc_word_hash(std::u8string_view word) {
  if (word.empty()) {
    return 0;
  }

  const std::size_t first = static_cast<std::size_t>(word[0]);
  const std::size_t last = static_cast<std::size_t>(word[word.length() - 1]);
  const std::size_t len = word.length();

  return ((first * 7) + (last * 13) + (len * 3)) % 128;
}

struct KeywordEntry {
  const char8_t* keyword_lexeme;
  std::size_t length;
  TokenKind token_kind;

  constexpr KeywordEntry(const char8_t* kw, TokenKind kind)
      : keyword_lexeme(kw),
        length(std::char_traits<char8_t>::length(kw)),
        token_kind(kind) {}

  constexpr KeywordEntry()
      : keyword_lexeme(nullptr),
        length(0),
        token_kind(TokenKind::kIdentifier) {}
};

consteval std::array<KeywordEntry, 128> create_keyword_table() {
  std::array<KeywordEntry, 128> table{};

  constexpr KeywordEntry keywords[] = {
      // # primitive types
      {u8"i8", TokenKind::kI8},
      {u8"i16", TokenKind::kI16},
      {u8"i32", TokenKind::kI32},
      {u8"i64", TokenKind::kI64},
      {u8"i128", TokenKind::kI128},
      {u8"isize", TokenKind::kIsize},
      {u8"u8", TokenKind::kU8},
      {u8"u16", TokenKind::kU16},
      {u8"u32", TokenKind::kU32},
      {u8"u64", TokenKind::kU64},
      {u8"u128", TokenKind::kU128},
      {u8"usize", TokenKind::kUsize},
      {u8"f32", TokenKind::kF32},
      {u8"f64", TokenKind::kF64},
      {u8"void", TokenKind::kVoid},
      {u8"byte", TokenKind::kByte},
      {u8"bool", TokenKind::kBool},
      {u8"char", TokenKind::kChar},
      {u8"str", TokenKind::kStr},

      // # qualified keywords

      // ## control flow
      {u8"if", TokenKind::kIf},
      {u8"else", TokenKind::kElse},
      {u8"loop", TokenKind::kLoop},
      {u8"while", TokenKind::kWhile},
      {u8"for", TokenKind::kFor},
      {u8"break", TokenKind::kBreak},
      {u8"continue", TokenKind::kContinue},
      {u8"ret", TokenKind::kReturn},
      {u8"match", TokenKind::kMatch},

      // ## declaration
      {u8"fn", TokenKind::kFunction},
      {u8"struct", TokenKind::kStruct},
      {u8"enum", TokenKind::kEnumeration},
      {u8"trait", TokenKind::kTrait},
      {u8"impl", TokenKind::kImplementation},
      {u8"union", TokenKind::kUnion},
      {u8"module", TokenKind::kModule},
      {u8"redirect", TokenKind::kRedirect},
      {u8"unsafe", TokenKind::kUnsafe},
      {u8"fast", TokenKind::kFast},

      // ## attribute
      {u8"mut", TokenKind::kMutable},
      {u8"const", TokenKind::kConstant},
      {u8"extern", TokenKind::kExtern},
      {u8"static", TokenKind::kStatic},
      {u8"thread_local", TokenKind::kThreadLocal},
      {u8"deprecated", TokenKind::kDeprecated},
      {u8"pub", TokenKind::kPublic},

      {u8"as", TokenKind::kAs},
      {u8"this", TokenKind::kThis},

      // ## literals
      {u8"true", TokenKind::kTrue},
      {u8"false", TokenKind::kFalse},
  };

  for (const auto& kw : keywords) {
    const std::size_t index = calc_word_hash(kw.keyword_lexeme);
    table[index] = kw;
  }

  return table;
}

constexpr auto kKeywordTable = create_keyword_table();

inline bool fast_equals(std::u8string_view word, const KeywordEntry& entry) {
  return word.length() == entry.length &&
         std::memcmp(word.data(), entry.keyword_lexeme, entry.length) == 0;
}

}  // namespace

TokenKind lookup_id_or_keyword(std::u8string_view word) {
  if (word.empty()) [[unlikely]] {
    return TokenKind::kIdentifier;
  }

  const std::size_t index = calc_word_hash(word);
  const KeywordEntry& entry = kKeywordTable[index];

  if (entry.keyword_lexeme != nullptr && fast_equals(word, entry)) [[likely]] {
    return entry.token_kind;
  }

  return TokenKind::kIdentifier;
}

TokenKind lookup_id_or_keyword(std::u8string_view full_source,
                               std::size_t start,
                               std::size_t length) {
  DCHECK_LE(start + length, full_source.length());
  return lookup_id_or_keyword(
      std::u8string_view(full_source.data() + start, length));
}

}  // namespace base
