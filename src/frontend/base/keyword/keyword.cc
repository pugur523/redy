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
      {u8"i8", TokenKind::kType},
      {u8"i16", TokenKind::kType},
      {u8"i32", TokenKind::kType},
      {u8"i64", TokenKind::kType},
      {u8"i128", TokenKind::kType},
      {u8"isize", TokenKind::kType},
      {u8"u8", TokenKind::kType},
      {u8"u16", TokenKind::kType},
      {u8"u32", TokenKind::kType},
      {u8"u64", TokenKind::kType},
      {u8"u128", TokenKind::kType},
      {u8"usize", TokenKind::kType},
      {u8"f32", TokenKind::kType},
      {u8"f64", TokenKind::kType},
      {u8"void", TokenKind::kType},
      {u8"bool", TokenKind::kType},
      {u8"char", TokenKind::kType},
      {u8"byte", TokenKind::kType},

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
      {u8"mut", TokenKind::kMutable},
      {u8"fn", TokenKind::kFunction},
      {u8"struct", TokenKind::kStruct},
      {u8"enum", TokenKind::kEnumeration},
      {u8"trait", TokenKind::kTrait},
      {u8"impl", TokenKind::kImplementation},
      {u8"union", TokenKind::kUnion},
      {u8"redirect", TokenKind::kRedirect},
      {u8"const", TokenKind::kConstant},
      {u8"extern", TokenKind::kExtern},
      {u8"static", TokenKind::kStatic},
      {u8"thread_local", TokenKind::kThreadLocal},

      {u8"unsafe", TokenKind::kUnsafe},
      {u8"fast", TokenKind::kFast},
      {u8"default", TokenKind::kDefault},
      {u8"deprecated", TokenKind::kDeprecated},
      {u8"pub", TokenKind::kPublic},

      {u8"as", TokenKind::kAs},
      {u8"this", TokenKind::kThis},

      // ## literals
      {u8"true", TokenKind::kTrue},
      {u8"false", TokenKind::kFalse},
  };

  for (const auto& kw : keywords) {
    std::size_t index = calc_word_hash(kw.keyword_lexeme);
    table[index] = kw;
  }

  return table;
}

constexpr auto kKeywordTable = create_keyword_table();

inline bool fast_equals(std::u8string_view word, const KeywordEntry& entry) {
  return word.length() == entry.length &&
         std::memcmp(word.data(), entry.keyword_lexeme, entry.length) == 0;
}

TokenKind fast_lookup(std::u8string_view word) {
  const std::size_t len = word.length();
  const char first = word[0];

  switch (len) {
    case 2:
      switch (first) {
        case 'i':
          if (word[1] == 'f') {
            return TokenKind::kIf;
          }
          if (word[1] == '8') {
            return TokenKind::kType;
          }
          break;
        case 'f':
          if (word[1] == 'n') {
            return TokenKind::kFunction;
          }
          break;
        case 'a':
          if (word[1] == 's') {
            return TokenKind::kAs;
          }
          break;
        case 'u':
          if (word[1] == '8') {
            return TokenKind::kType;
          }
          break;
      }
      break;

    case 3:
      switch (first) {
        case 'm':
          if (word == u8"mut") {
            return TokenKind::kMutable;
          }
          break;
        case 'f':
          if (word == u8"for") {
            return TokenKind::kFor;
          }
          if (word == u8"f32") {
            return TokenKind::kType;
          }
          if (word == u8"f64") {
            return TokenKind::kType;
          }
          break;
        case 'i':
          if (word == u8"i16") {
            return TokenKind::kType;
          }
          if (word == u8"i32") {
            return TokenKind::kType;
          }
          if (word == u8"i64") {
            return TokenKind::kType;
          }
          break;
        case 'u':
          if (word == u8"u16") {
            return TokenKind::kType;
          }
          if (word == u8"u32") {
            return TokenKind::kType;
          }
          if (word == u8"u64") {
            return TokenKind::kType;
          }
          break;
      }
      break;

    case 4:
      switch (first) {
        case 'e':
          if (word == u8"else") {
            return TokenKind::kElse;
          }
          if (word == u8"enum") {
            return TokenKind::kEnumeration;
          }
          break;
        case 't':
          if (word == u8"true") {
            return TokenKind::kTrue;
          }
          if (word == u8"this") {
            return TokenKind::kThis;
          }
          break;
        case 'v':
          if (word == u8"void") {
            return TokenKind::kType;
          }
          break;
        case 'b':
          if (word == u8"bool") {
            return TokenKind::kType;
          }
          if (word == u8"byte") {
            return TokenKind::kType;
          }
          break;
        case 'c':
          if (word == u8"char") {
            return TokenKind::kType;
          }
          break;
        case 'f':
          if (word == u8"fast") {
            return TokenKind::kFast;
          }
          break;
        case 'l':
          if (word == u8"loop") {
            return TokenKind::kLoop;
          }
        case 'i':
          if (word == u8"i128") {
            return TokenKind::kType;
          }
          if (word == u8"impl") {
            return TokenKind::kImplementation;
          }
          break;
        case 'u':
          if (word == u8"u128") {
            return TokenKind::kType;
          }
          break;
      }
      break;

    case 5:
      switch (first) {
        case 'w':
          if (word == u8"while") {
            return TokenKind::kWhile;
          }
          break;
        case 'b':
          if (word == u8"break") {
            return TokenKind::kBreak;
          }
          break;
        case 'f':
          if (word == u8"false") {
            return TokenKind::kFalse;
          }
          break;
        case 'c':
          if (word == u8"const") {
            return TokenKind::kConstant;
          }
        case 't':
          if (word == u8"trait") {
            return TokenKind::kTrait;
          }
        case 'm':
          if (word == u8"match") {
            return TokenKind::kMatch;
          }
        case 'i':
          if (word == u8"isize") {
            return TokenKind::kType;
          }
        case 'u':
          if (word == u8"union") {
            return TokenKind::kUnion;
          }
          if (word == u8"usize") {
            return TokenKind::kType;
          }
      }
      break;
  }

  return TokenKind::kIdentifier;
}

}  // namespace

TokenKind lookup_id_or_keyword(std::u8string_view word) {
  if (word.empty()) [[unlikely]] {
    return TokenKind::kIdentifier;
  }

  if (word.length() <= 5) [[likely]] {
    TokenKind result = fast_lookup(word);
    if (result != TokenKind::kIdentifier) {
      return result;
    }
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
