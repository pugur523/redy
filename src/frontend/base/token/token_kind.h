// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_TOKEN_TOKEN_KIND_H_
#define FRONTEND_BASE_TOKEN_TOKEN_KIND_H_

#include <cstdint>

#include "core/check.h"

namespace base {

// NOTE: when adding a new token kind, add it here and to
// `token_kind_to_string()`, and also to `keyword/*.[cc|h]` and
// `operator/*.[cc|h]` if neccessary.
enum class TokenKind : uint8_t {
  kUnknown = 0,

  kIdentifier = 1,

  // # keywords
  kType = 2,  // primitive types (e.g., i32, char, bool, byte)

  // ## control flow keywords
  kIf = 3,        // if
  kElse = 4,      // else
  kLoop = 5,      // loop
  kWhile = 6,     // while
  kFor = 7,       // for
  kBreak = 8,     // break
  kContinue = 9,  // continue
  kReturn = 10,   // ret
  kMatch = 11,    // match

  // ## declaration keywords
  kMutable = 12,         // mut
  kFunction = 13,        // fn
  kStruct = 14,          // struct
  kEnumeration = 15,     // enum
  kTrait = 16,           // trait
  kImplementation = 17,  // impl
  kUnion = 18,           // union
  kRedirect = 19,        // redirect
  kConstant = 20,        // const
  kExtern = 21,          // extern
  kStatic = 22,          // static
  kThreadLocal = 23,     // thread_local

  // ## attribute keywords
  kUnsafe = 24,      // unsafe
  kFast = 25,        // fast
  kDefault = 26,     // default
  kDeprecated = 27,  // deprecated
  kPublic = 28,      // pub

  // ## modifier keywords
  kThis = 29,  // this
  kAs = 30,    // as
  kKeywordsBegin = kType,
  kKeywordsEnd = kAs,

  // # literals
  kLiteralNumeric = 31,  // 42
  kLiteralStr = 32,      // "string"
  kLiteralChar = 33,     // 'c'
  kTrue = 34,            // true
  kFalse = 35,           // false
  kLiteralsBegin = kLiteralNumeric,
  kLiteralsEnd = kFalse,

  // # operators

  // ## unary
  kPlusPlus = 36,    // ++
  kMinusMinus = 37,  // --
  kBang = 38,        // !
  kTilde = 39,       // ~

  // ## exponentiation
  kStarStar = 40,  // **

  // ## multiplicative
  kStar = 41,     // *
  kSlash = 42,    // /
  kPercent = 43,  // %

  // ## additive
  kPlus = 44,   // +
  kMinus = 45,  // -

  // ## bitwise shift
  kLtLt = 46,  // <<
  kGtGt = 47,  // >>

  // ## comparisons
  kThreeWay = 48,  // <=>

  kLt = 49,  // <
  kGt = 50,  // >
  kLe = 51,  // <=
  kGe = 52,  // >=

  kEqEq = 53,      // ==
  kNotEqual = 54,  // !=

  // ## bitwise
  kAnd = 55,       // &
  kCaret = 56,     // ^
  kPipe = 57,      // |
  kAndAnd = 58,    // &&
  kPipePipe = 59,  // ||

  // ## assignment
  kAssign = 60,  // :=
  kEqual = 61,   // =

  // ## compound assignment
  kPlusEq = 62,     // +=
  kMinusEq = 63,    // -=
  kStarEq = 64,     // *=
  kSlashEq = 65,    // /=
  kPercentEq = 66,  // %=
  kAndEq = 67,      // &=
  kPipeEq = 68,     // |=
  kCaretEq = 69,    // ^=
  kLtLtEq = 70,     // <<=
  kGtGtEq = 71,     // >>=
  kOperatorsBegin = kPlusPlus,
  kOperatorsEnd = kGtGtEq,

  // # delimiters
  kArrow = 72,         // ->
  kColon = 73,         // :
  kColonColon = 74,    // ::
  kSemicolon = 75,     // ;
  kComma = 76,         // ,
  kDot = 77,           // .
  kDotDot = 78,        // ..
  kLeftParen = 79,     // (
  kRightParen = 80,    // )
  kLeftBrace = 81,     // {
  kRightBrace = 82,    // }
  kLeftBracket = 83,   // [
  kRightBracket = 84,  // ]
  kAt = 85,            // @
  kHash = 86,          // #
  kDollar = 87,        // $
  kQuestion = 88,      // ?
  kDelimitersBegin = kArrow,
  kDelimitersEnd = kQuestion,

  // # comment
  kInlineComment = 89,
  kBlockComment = 90,

  // # eof
  kEof = 91,
};

inline constexpr const char* token_kind_to_string(TokenKind kind) {
  // TODO: replace this with table
  switch (kind) {
    case TokenKind::kUnknown: return "unknown";

    case TokenKind::kIdentifier: return "identifier";
    case TokenKind::kType: return "type";

    case TokenKind::kIf: return "if";
    case TokenKind::kElse: return "else";
    case TokenKind::kLoop: return "loop";
    case TokenKind::kWhile: return "while";
    case TokenKind::kFor: return "for";
    case TokenKind::kBreak: return "break";
    case TokenKind::kContinue: return "continue";
    case TokenKind::kReturn: return "return";
    case TokenKind::kMatch: return "match";

    case TokenKind::kMutable: return "mutable";
    case TokenKind::kFunction: return "function";
    case TokenKind::kStruct: return "struct";
    case TokenKind::kEnumeration: return "enumeration";
    case TokenKind::kTrait: return "trait";
    case TokenKind::kImplementation: return "implementation";
    case TokenKind::kRedirect: return "redirect";
    case TokenKind::kUnion: return "union";
    case TokenKind::kConstant: return "constant";
    case TokenKind::kExtern: return "extern";
    case TokenKind::kStatic: return "static";
    case TokenKind::kThreadLocal: return "thread local";

    case TokenKind::kUnsafe: return "unsafe";
    case TokenKind::kFast: return "fast";
    case TokenKind::kDefault: return "default";
    case TokenKind::kDeprecated: return "deprecated";
    case TokenKind::kPublic: return "public";

    case TokenKind::kThis: return "this";
    case TokenKind::kAs: return "as";

    case TokenKind::kLiteralNumeric: return "literal numeric";
    case TokenKind::kLiteralStr: return "literal string";
    case TokenKind::kLiteralChar: return "literal char";
    case TokenKind::kTrue: return "true";
    case TokenKind::kFalse: return "false";

    case TokenKind::kPlusPlus: return "double plus";
    case TokenKind::kMinusMinus: return "double minus";
    case TokenKind::kBang: return "bang";
    case TokenKind::kTilde: return "tilde";

    case TokenKind::kStarStar: return "double star";

    case TokenKind::kStar: return "star";
    case TokenKind::kSlash: return "slash";
    case TokenKind::kPercent: return "percent";

    case TokenKind::kPlus: return "plus";
    case TokenKind::kMinus: return "minus";

    case TokenKind::kLtLt: return "double less than";
    case TokenKind::kGtGt: return "double greater than";

    case TokenKind::kThreeWay: return "three way";

    case TokenKind::kLt: return "less than";
    case TokenKind::kGt: return "greater than";
    case TokenKind::kLe: return "less than or equal";
    case TokenKind::kGe: return "greater than or equal";

    case TokenKind::kEqEq: return "double equal";
    case TokenKind::kNotEqual: return "not equal";

    case TokenKind::kAnd: return "and";
    case TokenKind::kCaret: return "caret";
    case TokenKind::kPipe: return "pipe";
    case TokenKind::kAndAnd: return "double and";
    case TokenKind::kPipePipe: return "double pipe";

    case TokenKind::kAssign: return "assign";
    case TokenKind::kEqual: return "equal";

    case TokenKind::kPlusEq: return "plus equal";
    case TokenKind::kMinusEq: return "minus equal";
    case TokenKind::kStarEq: return "star equal";
    case TokenKind::kSlashEq: return "slash equal";
    case TokenKind::kPercentEq: return "percent equal";
    case TokenKind::kAndEq: return "and equal";
    case TokenKind::kPipeEq: return "pipe equal";
    case TokenKind::kCaretEq: return "caret equal";
    case TokenKind::kLtLtEq: return "double less than equal";
    case TokenKind::kGtGtEq: return "double greater than equal";

    case TokenKind::kArrow: return "arrow";
    case TokenKind::kColon: return "colon";
    case TokenKind::kColonColon: return "double colon";
    case TokenKind::kSemicolon: return "semicolon";
    case TokenKind::kComma: return "comma";
    case TokenKind::kDot: return "dot";
    case TokenKind::kDotDot: return "double dot";
    case TokenKind::kLeftParen: return "left paren";
    case TokenKind::kRightParen: return "right paren";
    case TokenKind::kLeftBrace: return "left brace";
    case TokenKind::kRightBrace: return "right brace";
    case TokenKind::kLeftBracket: return "left bracket";
    case TokenKind::kRightBracket: return "right bracket";
    case TokenKind::kAt: return "at";
    case TokenKind::kHash: return "hash";
    case TokenKind::kDollar: return "dollar";
    case TokenKind::kQuestion: return "question";

    case TokenKind::kInlineComment: return "inline comment";
    case TokenKind::kBlockComment: return "block comment";

    case TokenKind::kEof: return "eof";
  }
}

inline bool is_operator(TokenKind kind) {
  return kind >= TokenKind::kOperatorsBegin && kind <= TokenKind::kOperatorsEnd;
}

}  // namespace base

#endif  // FRONTEND_BASE_TOKEN_TOKEN_KIND_H_
