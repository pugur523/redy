// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_TOKEN_TOKEN_KIND_H_
#define FRONTEND_BASE_TOKEN_TOKEN_KIND_H_

#include <cstdint>

#include "core/check.h"
#include "i18n/base/data/translation_key.h"

namespace base {

// NOTE: when adding a new token kind, add it here and to
// `token_kind_to_string()`, `token_kind_to_tr_key()`,
// `//src/build/resources/i18n/*/term.toml`, and base/*/*.[h|cc] if neccessary
enum class TokenKind : uint8_t {
  kUnknown = 0,

  kIdentifier = 1,

  // # keywords
  // ## primitive types
  kI8 = 2,
  kI16 = 3,
  kI32 = 4,
  kI64 = 5,
  kI128 = 6,
  kIsize = 7,
  kU8 = 8,
  kU16 = 9,
  kU32 = 10,
  kU64 = 11,
  kU128 = 12,
  kUsize = 13,
  kF32 = 14,
  kF64 = 15,
  kVoid = 16,
  kByte = 17,
  kBool = 18,
  kChar = 19,
  kStr = 20,

  // ## control flow keywords
  kIf = 21,        // if
  kElse = 22,      // else
  kLoop = 23,      // loop
  kWhile = 24,     // while
  kFor = 25,       // for
  kBreak = 26,     // break
  kContinue = 27,  // continue
  kReturn = 28,    // ret
  kMatch = 29,     // match

  // ## declaration keywords
  kFunction = 30,        // fn
  kStruct = 31,          // struct
  kEnumeration = 32,     // enum
  kTrait = 33,           // trait
  kImplementation = 34,  // impl
  kUnion = 35,           // union
  kModule = 36,          // mod
  kRedirect = 37,        // redirect

  // ## uncategorized
  kUse = 38,  // use

  // ## attribute keywords
  kMutable = 39,      // mut
  kConstant = 40,     // const
  kExtern = 41,       // extern
  kStatic = 42,       // static
  kThreadLocal = 43,  // thread_local
  kPublic = 44,       // pub
  kAsync = 45,        // async
  kUnsafe = 46,       // unsafe
  kFast = 47,         // fast

  // ## modifier keywords
  kThis = 48,   // this
  kAs = 49,     // as
  kAwait = 50,  // await
  kKeywordsBegin = kI8,
  kKeywordsEnd = kAwait,

  // # literals
  kDecimal = 51,      // 42
  kBinary = 52,       // 0b01010
  kOctal = 53,        // 0o755
  kHexadecimal = 54,  // 0xffffff
  kString = 55,       // "string"
  kCharacter = 56,    // 'c'
  kTrue = 57,         // true
  kFalse = 58,        // false
  kLiteralsBegin = kDecimal,
  kLiteralsEnd = kFalse,

  // # operators

  // ## unary
  kPlusPlus = 59,    // ++
  kMinusMinus = 60,  // --
  kBang = 61,        // !
  kTilde = 62,       // ~

  // ## exponentiation
  kStarStar = 63,  // **

  // ## multiplicative
  kStar = 64,     // *
  kSlash = 65,    // /
  kPercent = 66,  // %

  // ## additive
  kPlus = 67,   // +
  kMinus = 68,  // -

  // ## bitwise shift
  kLtLt = 69,  // <<
  kGtGt = 70,  // >>

  // ## comparisons
  kThreeWay = 71,  // <=>

  kLt = 72,  // <
  kGt = 73,  // >
  kLe = 74,  // <=
  kGe = 75,  // >=

  kEqEq = 76,      // ==
  kNotEqual = 77,  // !=

  // ## bitwise
  kAnd = 78,       // &
  kCaret = 79,     // ^
  kPipe = 80,      // |
  kAndAnd = 81,    // &&
  kPipePipe = 82,  // ||

  // ## assignment
  kColonEqual = 83,  // :=
  kEqual = 84,       // =

  // ## compound assignment
  kPlusEq = 85,     // +=
  kMinusEq = 86,    // -=
  kStarEq = 87,     // *=
  kSlashEq = 88,    // /=
  kPercentEq = 89,  // %=
  kAndEq = 90,      // &=
  kPipeEq = 91,     // |=
  kCaretEq = 92,    // ^=
  kLtLtEq = 93,     // <<=
  kGtGtEq = 94,     // >>=
  kOperatorsBegin = kPlusPlus,
  kOperatorsEnd = kGtGtEq,

  // # delimiters
  kArrow = 95,          // ->
  kColon = 96,          // :
  kColonColon = 97,     // ::
  kSemicolon = 98,      // ;
  kComma = 99,          // ,
  kDot = 100,           // .
  kDotDot = 101,        // ..
  kLeftParen = 102,     // (
  kRightParen = 103,    // )
  kLeftBrace = 104,     // {
  kRightBrace = 105,    // }
  kLeftBracket = 106,   // [
  kRightBracket = 107,  // ]
  kAt = 108,            // @
  kHash = 109,          // #
  kDollar = 110,        // $
  kQuestion = 111,      // ?
  kDelimitersBegin = kArrow,
  kDelimitersEnd = kQuestion,

  // # whitespace
  kWhitespace = 112,  // includes unicode whitespaces
  kNewline = 113,     // includes unicode newlines
  //
  // # comment
  kInlineComment = 114,         // // inline comment
  kBlockComment = 115,          // /* block comment */
  kDocumentationComment = 116,  // /// documetation comment

  // # eof
  kEof = 117,  // \0
};

inline constexpr const char* token_kind_to_string(TokenKind kind) {
  // TODO: replace this with table
  switch (kind) {
    case TokenKind::kUnknown: return "unknown";

    case TokenKind::kIdentifier: return "identifier";

    case TokenKind::kI8: return "i8";
    case TokenKind::kI16: return "i16";
    case TokenKind::kI32: return "i32";
    case TokenKind::kI64: return "i64";
    case TokenKind::kI128: return "i128";
    case TokenKind::kIsize: return "isize";
    case TokenKind::kU8: return "u8";
    case TokenKind::kU16: return "u16";
    case TokenKind::kU32: return "u32";
    case TokenKind::kU64: return "u64";
    case TokenKind::kU128: return "u128";
    case TokenKind::kUsize: return "usize";
    case TokenKind::kF32: return "f32";
    case TokenKind::kF64: return "f64";
    case TokenKind::kVoid: return "void";
    case TokenKind::kByte: return "byte";
    case TokenKind::kBool: return "bool";
    case TokenKind::kChar: return "char";
    case TokenKind::kStr: return "str";

    case TokenKind::kIf: return "if";
    case TokenKind::kElse: return "else";
    case TokenKind::kLoop: return "loop";
    case TokenKind::kWhile: return "while";
    case TokenKind::kFor: return "for";
    case TokenKind::kBreak: return "break";
    case TokenKind::kContinue: return "continue";
    case TokenKind::kReturn: return "return";
    case TokenKind::kMatch: return "match";

    case TokenKind::kFunction: return "function";
    case TokenKind::kStruct: return "struct";
    case TokenKind::kEnumeration: return "enumeration";
    case TokenKind::kTrait: return "trait";
    case TokenKind::kImplementation: return "implementation";
    case TokenKind::kUnion: return "union";
    case TokenKind::kModule: return "module";
    case TokenKind::kRedirect: return "redirect";
    case TokenKind::kUse: return "use";

    case TokenKind::kMutable: return "mutable";
    case TokenKind::kConstant: return "constant";
    case TokenKind::kExtern: return "extern";
    case TokenKind::kStatic: return "static";
    case TokenKind::kThreadLocal: return "thread local";
    case TokenKind::kPublic: return "public";
    case TokenKind::kAsync: return "async";
    case TokenKind::kUnsafe: return "unsafe";
    case TokenKind::kFast: return "fast";

    case TokenKind::kThis: return "this";
    case TokenKind::kAs: return "as";
    case TokenKind::kAwait: return "await";

    case TokenKind::kDecimal: return "literal decimal";
    case TokenKind::kBinary: return "literal binary";
    case TokenKind::kOctal: return "literal octal";
    case TokenKind::kHexadecimal: return "literal hexadecimal";
    case TokenKind::kString: return "literal string";
    case TokenKind::kCharacter: return "literal char";
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

    case TokenKind::kColonEqual: return "assign";
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

    case TokenKind::kWhitespace: return "whitespace";
    case TokenKind::kNewline: return "newline";

    case TokenKind::kInlineComment: return "inline comment";
    case TokenKind::kBlockComment: return "block comment";
    case TokenKind::kDocumentationComment: return "documentation comment";

    case TokenKind::kEof: return "eof";
  }
}

inline constexpr i18n::TranslationKey token_kind_to_tr_key(TokenKind kind) {
  switch (kind) {
    case TokenKind::kUnknown:
      return i18n::TranslationKey::kTermTokenKindUnknown;

    case TokenKind::kIdentifier:
      return i18n::TranslationKey::kTermTokenKindIdentifier;

    case TokenKind::kI8: return i18n::TranslationKey::kTermTokenKindI8;
    case TokenKind::kI16: return i18n::TranslationKey::kTermTokenKindI16;
    case TokenKind::kI32: return i18n::TranslationKey::kTermTokenKindI32;
    case TokenKind::kI64: return i18n::TranslationKey::kTermTokenKindI64;
    case TokenKind::kI128: return i18n::TranslationKey::kTermTokenKindI128;
    case TokenKind::kIsize: return i18n::TranslationKey::kTermTokenKindIsize;
    case TokenKind::kU8: return i18n::TranslationKey::kTermTokenKindU8;
    case TokenKind::kU16: return i18n::TranslationKey::kTermTokenKindU16;
    case TokenKind::kU32: return i18n::TranslationKey::kTermTokenKindU32;
    case TokenKind::kU64: return i18n::TranslationKey::kTermTokenKindU64;
    case TokenKind::kU128: return i18n::TranslationKey::kTermTokenKindU128;
    case TokenKind::kUsize: return i18n::TranslationKey::kTermTokenKindUsize;
    case TokenKind::kF32: return i18n::TranslationKey::kTermTokenKindF32;
    case TokenKind::kF64: return i18n::TranslationKey::kTermTokenKindF64;
    case TokenKind::kVoid: return i18n::TranslationKey::kTermTokenKindVoid;
    case TokenKind::kByte: return i18n::TranslationKey::kTermTokenKindByte;
    case TokenKind::kBool: return i18n::TranslationKey::kTermTokenKindBool;
    case TokenKind::kChar: return i18n::TranslationKey::kTermTokenKindChar;
    case TokenKind::kStr: return i18n::TranslationKey::kTermTokenKindStr;

    case TokenKind::kIf: return i18n::TranslationKey::kTermTokenKindIf;
    case TokenKind::kElse: return i18n::TranslationKey::kTermTokenKindElse;
    case TokenKind::kLoop: return i18n::TranslationKey::kTermTokenKindLoop;
    case TokenKind::kWhile: return i18n::TranslationKey::kTermTokenKindWhile;
    case TokenKind::kFor: return i18n::TranslationKey::kTermTokenKindFor;
    case TokenKind::kBreak: return i18n::TranslationKey::kTermTokenKindBreak;
    case TokenKind::kContinue:
      return i18n::TranslationKey::kTermTokenKindContinue;
    case TokenKind::kReturn: return i18n::TranslationKey::kTermTokenKindReturn;
    case TokenKind::kMatch: return i18n::TranslationKey::kTermTokenKindMatch;

    case TokenKind::kFunction:
      return i18n::TranslationKey::kTermTokenKindFunction;
    case TokenKind::kStruct: return i18n::TranslationKey::kTermTokenKindStruct;
    case TokenKind::kEnumeration:
      return i18n::TranslationKey::kTermTokenKindEnumeration;
    case TokenKind::kTrait: return i18n::TranslationKey::kTermTokenKindTrait;
    case TokenKind::kImplementation:
      return i18n::TranslationKey::kTermTokenKindImplementation;
    case TokenKind::kUnion: return i18n::TranslationKey::kTermTokenKindUnion;
    case TokenKind::kModule: return i18n::TranslationKey::kTermTokenKindModule;
    case TokenKind::kRedirect:
      return i18n::TranslationKey::kTermTokenKindRedirect;
    case TokenKind::kUse: return i18n::TranslationKey::kTermTokenKindUse;

    case TokenKind::kMutable:
      return i18n::TranslationKey::kTermTokenKindMutable;
    case TokenKind::kConstant:
      return i18n::TranslationKey::kTermTokenKindConstant;
    case TokenKind::kExtern: return i18n::TranslationKey::kTermTokenKindExtern;
    case TokenKind::kStatic: return i18n::TranslationKey::kTermTokenKindStatic;
    case TokenKind::kThreadLocal:
      return i18n::TranslationKey::kTermTokenKindThreadLocal;
    case TokenKind::kPublic: return i18n::TranslationKey::kTermTokenKindPublic;
    case TokenKind::kAsync: return i18n::TranslationKey::kTermTokenKindAsync;
    case TokenKind::kUnsafe: return i18n::TranslationKey::kTermTokenKindUnsafe;
    case TokenKind::kFast: return i18n::TranslationKey::kTermTokenKindFast;

    case TokenKind::kThis: return i18n::TranslationKey::kTermTokenKindThis;
    case TokenKind::kAs: return i18n::TranslationKey::kTermTokenKindAs;
    case TokenKind::kAwait: return i18n::TranslationKey::kTermTokenKindAwait;

    case TokenKind::kDecimal:
      return i18n::TranslationKey::kTermTokenKindLiteralDecimal;
    case TokenKind::kBinary:
      return i18n::TranslationKey::kTermTokenKindLiteralBinary;
    case TokenKind::kOctal:
      return i18n::TranslationKey::kTermTokenKindLiteralOctal;
    case TokenKind::kHexadecimal:
      return i18n::TranslationKey::kTermTokenKindLiteralHexadecimal;
    case TokenKind::kString:
      return i18n::TranslationKey::kTermTokenKindLiteralStr;
    case TokenKind::kCharacter:
      return i18n::TranslationKey::kTermTokenKindLiteralChar;
    case TokenKind::kTrue: return i18n::TranslationKey::kTermTokenKindTrue;
    case TokenKind::kFalse: return i18n::TranslationKey::kTermTokenKindFalse;

    case TokenKind::kPlusPlus:
      return i18n::TranslationKey::kTermTokenKindPlusPlus;
    case TokenKind::kMinusMinus:
      return i18n::TranslationKey::kTermTokenKindMinusMinus;
    case TokenKind::kBang: return i18n::TranslationKey::kTermTokenKindBang;
    case TokenKind::kTilde: return i18n::TranslationKey::kTermTokenKindTilde;

    case TokenKind::kStarStar:
      return i18n::TranslationKey::kTermTokenKindStarStar;

    case TokenKind::kStar: return i18n::TranslationKey::kTermTokenKindStar;
    case TokenKind::kSlash: return i18n::TranslationKey::kTermTokenKindSlash;
    case TokenKind::kPercent:
      return i18n::TranslationKey::kTermTokenKindPercent;

    case TokenKind::kPlus: return i18n::TranslationKey::kTermTokenKindPlus;
    case TokenKind::kMinus: return i18n::TranslationKey::kTermTokenKindMinus;

    case TokenKind::kLtLt: return i18n::TranslationKey::kTermTokenKindLtLt;
    case TokenKind::kGtGt: return i18n::TranslationKey::kTermTokenKindGtGt;

    case TokenKind::kThreeWay:
      return i18n::TranslationKey::kTermTokenKindThreeWay;

    case TokenKind::kLt: return i18n::TranslationKey::kTermTokenKindLt;
    case TokenKind::kGt: return i18n::TranslationKey::kTermTokenKindGt;
    case TokenKind::kLe: return i18n::TranslationKey::kTermTokenKindLe;
    case TokenKind::kGe: return i18n::TranslationKey::kTermTokenKindGe;

    case TokenKind::kEqEq: return i18n::TranslationKey::kTermTokenKindEqEq;
    case TokenKind::kNotEqual:
      return i18n::TranslationKey::kTermTokenKindNotEqual;

    case TokenKind::kAnd: return i18n::TranslationKey::kTermTokenKindAnd;
    case TokenKind::kCaret: return i18n::TranslationKey::kTermTokenKindCaret;
    case TokenKind::kPipe: return i18n::TranslationKey::kTermTokenKindPipe;
    case TokenKind::kAndAnd: return i18n::TranslationKey::kTermTokenKindAndAnd;
    case TokenKind::kPipePipe:
      return i18n::TranslationKey::kTermTokenKindPipePipe;

    case TokenKind::kColonEqual:
      return i18n::TranslationKey::kTermTokenKindColonEqual;
    case TokenKind::kEqual: return i18n::TranslationKey::kTermTokenKindEqual;

    case TokenKind::kPlusEq: return i18n::TranslationKey::kTermTokenKindPlusEq;
    case TokenKind::kMinusEq:
      return i18n::TranslationKey::kTermTokenKindMinusEq;
    case TokenKind::kStarEq: return i18n::TranslationKey::kTermTokenKindStarEq;
    case TokenKind::kSlashEq:
      return i18n::TranslationKey::kTermTokenKindSlashEq;
    case TokenKind::kPercentEq:
      return i18n::TranslationKey::kTermTokenKindPercentEq;
    case TokenKind::kAndEq: return i18n::TranslationKey::kTermTokenKindAndEq;
    case TokenKind::kPipeEq: return i18n::TranslationKey::kTermTokenKindPipeEq;
    case TokenKind::kCaretEq:
      return i18n::TranslationKey::kTermTokenKindCaretEq;
    case TokenKind::kLtLtEq: return i18n::TranslationKey::kTermTokenKindLtLtEq;
    case TokenKind::kGtGtEq: return i18n::TranslationKey::kTermTokenKindGtGtEq;

    case TokenKind::kArrow: return i18n::TranslationKey::kTermTokenKindArrow;
    case TokenKind::kColon: return i18n::TranslationKey::kTermTokenKindColon;
    case TokenKind::kColonColon:
      return i18n::TranslationKey::kTermTokenKindColonColon;
    case TokenKind::kSemicolon:
      return i18n::TranslationKey::kTermTokenKindSemicolon;
    case TokenKind::kComma: return i18n::TranslationKey::kTermTokenKindComma;
    case TokenKind::kDot: return i18n::TranslationKey::kTermTokenKindDot;
    case TokenKind::kDotDot: return i18n::TranslationKey::kTermTokenKindDotDot;
    case TokenKind::kLeftParen:
      return i18n::TranslationKey::kTermTokenKindLeftParen;
    case TokenKind::kRightParen:
      return i18n::TranslationKey::kTermTokenKindRightParen;
    case TokenKind::kLeftBrace:
      return i18n::TranslationKey::kTermTokenKindLeftBrace;
    case TokenKind::kRightBrace:
      return i18n::TranslationKey::kTermTokenKindRightBrace;
    case TokenKind::kLeftBracket:
      return i18n::TranslationKey::kTermTokenKindLeftBracket;
    case TokenKind::kRightBracket:
      return i18n::TranslationKey::kTermTokenKindRightBracket;
    case TokenKind::kAt: return i18n::TranslationKey::kTermTokenKindAt;
    case TokenKind::kHash: return i18n::TranslationKey::kTermTokenKindHash;
    case TokenKind::kDollar: return i18n::TranslationKey::kTermTokenKindDollar;
    case TokenKind::kQuestion:
      return i18n::TranslationKey::kTermTokenKindQuestion;

    case TokenKind::kWhitespace:
      return i18n::TranslationKey::kTermTokenKindWhitespace;
    case TokenKind::kNewline:
      return i18n::TranslationKey::kTermTokenKindNewline;

    case TokenKind::kInlineComment:
      return i18n::TranslationKey::kTermTokenKindInlineComment;
    case TokenKind::kBlockComment:
      return i18n::TranslationKey::kTermTokenKindBlockComment;
    case TokenKind::kDocumentationComment:
      return i18n::TranslationKey::kTermTokenKindDocumentationComment;

    case TokenKind::kEof: return i18n::TranslationKey::kTermTokenKindEof;
  }
}

inline bool is_operator(TokenKind kind) {
  return TokenKind::kOperatorsBegin <= kind && kind <= TokenKind::kOperatorsEnd;
}

}  // namespace base

#endif  // FRONTEND_BASE_TOKEN_TOKEN_KIND_H_
