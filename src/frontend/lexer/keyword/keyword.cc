// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/lexer/keyword/keyword.h"

#include <array>
#include <cstring>
#include <string>
#include <string_view>

#include "frontend/lexer/token/token.h"

namespace lexer {

namespace {

constexpr std::size_t calc_hash(std::string_view word) {
  if (word.empty()) {
    return 0;
  }

  const std::size_t first = static_cast<std::size_t>(word[0]);
  const std::size_t last = static_cast<std::size_t>(word[word.length() - 1]);
  const std::size_t len = word.length();

  return ((first * 7) + (last * 13) + (len * 3)) % 128;
}

struct KeywordEntry {
  const char* keyword;
  std::size_t length;
  TokenKind token_kind;

  constexpr KeywordEntry(const char* kw, TokenKind kind)
      : keyword(kw),
        length(std::char_traits<char>::length(kw)),
        token_kind(kind) {}

  constexpr KeywordEntry()
      : keyword(nullptr), length(0), token_kind(TokenKind::kIdentifier) {}
};

constexpr std::array<KeywordEntry, 128> create_hash_table() {
  std::array<KeywordEntry, 128> table{};

  constexpr KeywordEntry keywords[] = {
      {"mut", TokenKind::kMut},
      {"fn", TokenKind::kFn},
      {"return", TokenKind::kReturn},
      {"if", TokenKind::kIf},
      {"else", TokenKind::kElse},
      {"while", TokenKind::kWhile},
      {"for", TokenKind::kFor},
      {"break", TokenKind::kBreak},
      {"continue", TokenKind::kContinue},
      {"struct", TokenKind::kStruct},
      {"enum", TokenKind::kEnum},
      {"as", TokenKind::kAs},
      {"static", TokenKind::kStatic},
      {"this", TokenKind::kThis},
      {"extern", TokenKind::kExtern},
      {"unsafe", TokenKind::kUnsafe},
      {"fast", TokenKind::kFast},
      {"default", TokenKind::kDefault},
      {"deprecated", TokenKind::kDeprecated},
      {"true", TokenKind::kTrue},
      {"false", TokenKind::kFalse},
      {"i8", TokenKind::kType},
      {"i16", TokenKind::kType},
      {"i32", TokenKind::kType},
      {"i64", TokenKind::kType},
      {"i128", TokenKind::kType},
      {"isize", TokenKind::kType},
      {"u8", TokenKind::kType},
      {"u16", TokenKind::kType},
      {"u32", TokenKind::kType},
      {"u64", TokenKind::kType},
      {"u128", TokenKind::kType},
      {"usize", TokenKind::kType},
      {"f32", TokenKind::kType},
      {"f64", TokenKind::kType},
      {"void", TokenKind::kType},
      {"bool", TokenKind::kType},
      {"char", TokenKind::kType},
      {"string", TokenKind::kType},
  };

  for (const auto& kw : keywords) {
    std::size_t index = calc_hash(kw.keyword);
    table[index] = kw;
  }

  return table;
}

constexpr auto kPerfectHashTable = create_hash_table();

inline bool optimized_equals(std::string_view word,
                             const KeywordEntry& entry) noexcept {
  return word.length() == entry.length &&
         std::memcmp(word.data(), entry.keyword, entry.length) == 0;
}

TokenKind optimized_lookup(std::string_view word) noexcept {
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
            return TokenKind::kFn;
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
          if (word == "mut") {
            return TokenKind::kMut;
          }
          break;
        case 'f':
          if (word == "for") {
            return TokenKind::kFor;
          }
          if (word == "f32") {
            return TokenKind::kType;
          }
          if (word == "f64") {
            return TokenKind::kType;
          }
          break;
        case 'i':
          if (word == "i16") {
            return TokenKind::kType;
          }
          if (word == "i32") {
            return TokenKind::kType;
          }
          if (word == "i64") {
            return TokenKind::kType;
          }
          break;
        case 'u':
          if (word == "u16") {
            return TokenKind::kType;
          }
          if (word == "u32") {
            return TokenKind::kType;
          }
          if (word == "u64") {
            return TokenKind::kType;
          }
          break;
      }
      break;

    case 4:
      switch (first) {
        case 'e':
          if (word == "else") {
            return TokenKind::kElse;
          }
          if (word == "enum") {
            return TokenKind::kEnum;
          }
          break;
        case 't':
          if (word == "true") {
            return TokenKind::kTrue;
          }
          if (word == "this") {
            return TokenKind::kThis;
          }
          break;
        case 'v':
          if (word == "void") {
            return TokenKind::kType;
          }
          break;
        case 'b':
          if (word == "bool") {
            return TokenKind::kType;
          }
          break;
        case 'c':
          if (word == "char") {
            return TokenKind::kType;
          }
          break;
        case 'f':
          if (word == "fast") {
            return TokenKind::kFast;
          }
          break;
      }
      break;

    case 5:
      switch (first) {
        case 'w':
          if (word == "while") {
            return TokenKind::kWhile;
          }
          break;
        case 'b':
          if (word == "break") {
            return TokenKind::kBreak;
          }
          break;
        case 'f':
          if (word == "false") {
            return TokenKind::kFalse;
          }
          break;
        case 'i':
          if (word == "i128") {
            return TokenKind::kType;
          }
          break;
        case 'u':
          if (word == "u128") {
            return TokenKind::kType;
          }
          break;
      }
      break;
  }

  return TokenKind::kIdentifier;
}

}  // namespace

TokenKind lookup_identifier_or_keyword(const std::string_view& word) {
  if (word.empty()) [[unlikely]] {
    return TokenKind::kIdentifier;
  }

  if (word.length() <= 5) [[likely]] {
    TokenKind result = optimized_lookup(word);
    if (result != TokenKind::kIdentifier) {
      return result;
    }
  }

  const std::size_t index = calc_hash(word);
  const KeywordEntry& entry = kPerfectHashTable[index];

  if (entry.keyword != nullptr && optimized_equals(word, entry)) [[likely]] {
    return entry.token_kind;
  }

  return TokenKind::kIdentifier;
}

TokenKind lookup_identifier_or_keyword(const std::string& full_source,
                                       std::size_t start,
                                       std::size_t length) {
  DCHECK_LE(start + length, full_source.length());
  return lookup_identifier_or_keyword(
      std::string_view(full_source.data() + start, length));
}

}  // namespace lexer
