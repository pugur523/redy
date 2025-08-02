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

constexpr std::size_t calc_word_hash(std::string_view word) {
  if (word.empty()) {
    return 0;
  }

  const std::size_t first = static_cast<std::size_t>(word[0]);
  const std::size_t last = static_cast<std::size_t>(word[word.length() - 1]);
  const std::size_t len = word.length();

  return ((first * 7) + (last * 13) + (len * 3)) % 128;
}

struct KeywordEntry {
  const char* keyword_lexeme;
  std::size_t length;
  TokenKind token_kind;

  constexpr KeywordEntry(const char* kw, TokenKind kind)
      : keyword_lexeme(kw),
        length(std::char_traits<char>::length(kw)),
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
      {"byte", TokenKind::kType},

      // # qualified keywords

      // ## control flow
      {"if", TokenKind::kIf},
      {"else", TokenKind::kElse},
      {"loop", TokenKind::kLoop},
      {"while", TokenKind::kWhile},
      {"for", TokenKind::kFor},
      {"break", TokenKind::kBreak},
      {"continue", TokenKind::kContinue},
      {"ret", TokenKind::kReturn},
      {"match", TokenKind::kMatch},

      // ## declaration
      {"mut", TokenKind::kMutable},
      {"fn", TokenKind::kFunction},
      {"struct", TokenKind::kStruct},
      {"enum", TokenKind::kEnumeration},
      {"trait", TokenKind::kTrait},
      {"impl", TokenKind::kImplementation},
      {"union", TokenKind::kUnion},
      {"redirect", TokenKind::kRedirect},
      {"const", TokenKind::kConstant},
      {"extern", TokenKind::kExtern},
      {"static", TokenKind::kStatic},
      {"thread_local", TokenKind::kThreadLocal},

      {"unsafe", TokenKind::kUnsafe},
      {"fast", TokenKind::kFast},
      {"default", TokenKind::kDefault},
      {"deprecated", TokenKind::kDeprecated},
      {"pub", TokenKind::kPublic},

      {"as", TokenKind::kAs},
      {"this", TokenKind::kThis},

      // ## literals
      {"true", TokenKind::kTrue},
      {"false", TokenKind::kFalse},
  };

  for (const auto& kw : keywords) {
    std::size_t index = calc_word_hash(kw.keyword_lexeme);
    table[index] = kw;
  }

  return table;
}

constexpr auto kKeywordTable = create_keyword_table();

inline bool fast_equals(std::string_view word, const KeywordEntry& entry) {
  return word.length() == entry.length &&
         std::memcmp(word.data(), entry.keyword_lexeme, entry.length) == 0;
}

TokenKind fast_lookup(std::string_view word) {
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
          if (word == "mut") {
            return TokenKind::kMutable;
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
            return TokenKind::kEnumeration;
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
          if (word == "byte") {
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
        case 'l':
          if (word == "loop") {
            return TokenKind::kLoop;
          }
        case 'i':
          if (word == "i128") {
            return TokenKind::kType;
          }
          if (word == "impl") {
            return TokenKind::kImplementation;
          }
          break;
        case 'u':
          if (word == "u128") {
            return TokenKind::kType;
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
        case 'c':
          if (word == "const") {
            return TokenKind::kConstant;
          }
        case 't':
          if (word == "trait") {
            return TokenKind::kTrait;
          }
        case 'm':
          if (word == "match") {
            return TokenKind::kMatch;
          }
        case 'i':
          if (word == "isize") {
            return TokenKind::kType;
          }
        case 'u':
          if (word == "union") {
            return TokenKind::kUnion;
          }
          if (word == "usize") {
            return TokenKind::kType;
          }
      }
      break;
  }

  return TokenKind::kIdentifier;
}

}  // namespace

TokenKind lookup_id_or_keyword(std::string_view word) {
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

TokenKind lookup_id_or_keyword(const std::string& full_source,
                               std::size_t start,
                               std::size_t length) {
  DCHECK_LE(start + length, full_source.length());
  return lookup_id_or_keyword(
      std::string_view(full_source.data() + start, length));
}

}  // namespace base
