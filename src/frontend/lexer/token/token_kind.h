// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_LEXER_TOKEN_TOKEN_KIND_H_
#define FRONTEND_LEXER_TOKEN_TOKEN_KIND_H_

#include <cstdint>

namespace lexer {

enum class TokenKind : uint8_t {
  // Keywords, see also frontend/lexer/keyword/keyword.cc
  kIdentifier = 0,
  kType = 1,
  kMut = 3,
  kFn = 4,
  kIf = 5,
  kElse = 6,
  kWhile = 7,
  kReturn = 8,
  kFor = 9,
  kBreak = 10,
  kContinue = 11,
  kStruct = 12,
  kEnum = 13,
  kAs = 14,
  kStatic = 15,
  kThis = 16,
  kExtern = 17,
  kUnsafe = 18,
  kFast = 19,
  kDefault = 20,
  kDeprecated = 21,

  // Literals
  kLiteralNumber = 22,
  kLiteralString = 23,  // "string"
  kLiteralChar = 24,    // 'c'
  kTrue = 25,           // boolean true
  kFalse = 26,          // boolean false

  // Operators
  kPlus = 27,        // +
  kMinus = 28,       // -
  kStar = 29,        // *
  kSlash = 30,       // /
  kPercent = 31,     // % (modulo)
  kStarStar = 32,    // ** (power)
  kPlusPlus = 33,    // ++ (increment)
  kMinusMinus = 34,  // -- (decrement)
  kAssign = 35,      // =
  kLet = 36,         // :=
  kEqEq = 37,        // ==
  kNeq = 38,         // !=
  kLt = 39,          // <
  kGt = 40,          // >
  kLe = 41,          // <=
  kGe = 42,          // >=
  kBang = 43,        // !
  kAndAnd = 44,      // && (logical AND)
  kPipePipe = 45,    // || (logical OR)

  // Bitwise Operators
  kAmp = 46,    // & (bitwise AND)
  kPipe = 47,   // | (bitwise OR)
  kCaret = 48,  // ^ (bitwise XOR)
  kTilde = 49,  // ~ (bitwise NOT)
  kLtLt = 50,   // << (left shift)
  kGtGt = 51,   // >> (right shift)

  // Compound Assignment Operators
  kPlusEq = 52,     // +=
  kMinusEq = 53,    // -=
  kStarEq = 54,     // *=
  kSlashEq = 55,    // /=
  kPercentEq = 56,  // %=
  kAmpEq = 57,      // &=
  kPipeEq = 58,     // |=
  kCaretEq = 59,    // ^=
  kLtLtEq = 60,     // <<=
  kGtGtEq = 61,     // >>=

  // Delimiters
  kArrow = 62,       // ->
  kColon = 63,       // :
  kColonColon = 64,  // ::
  kSemicolon = 65,   // ;
  kComma = 66,       // ,
  kDot = 67,         // .
  kDotDot = 68,      // .. (range operator)
  kLParen = 69,      // (
  kRParen = 70,      // )
  kLBrace = 71,      // {
  kRBrace = 72,      // }
  kLBracket = 73,    // [
  kRBracket = 74,    // ]
  kAt = 75,          // @
  kHash = 76,        // #
  kDollar = 77,      // $
  kQuestion = 78,    // ?

  kEof = 79,
  kUnknown = 80,
};

inline constexpr const char* to_string(TokenKind kind) {
  switch (kind) {
    case TokenKind::kIdentifier: return "identifier";
    case TokenKind::kType: return "type";
    case TokenKind::kMut: return "mut";
    case TokenKind::kFn: return "fn";
    case TokenKind::kIf: return "if";
    case TokenKind::kElse: return "else";
    case TokenKind::kWhile: return "while";
    case TokenKind::kReturn: return "return";
    case TokenKind::kFor: return "for";
    case TokenKind::kBreak: return "break";
    case TokenKind::kContinue: return "continue";
    case TokenKind::kStruct: return "struct";
    case TokenKind::kEnum: return "enum";
    case TokenKind::kAs: return "as";
    case TokenKind::kStatic: return "static";
    case TokenKind::kThis: return "this";
    case TokenKind::kExtern: return "extern";
    case TokenKind::kUnsafe: return "unsafe";
    case TokenKind::kFast: return "fast";
    case TokenKind::kDefault: return "defualt";
    case TokenKind::kDeprecated: return "deprecated";

    case TokenKind::kLiteralNumber: return "literal number";
    case TokenKind::kLiteralString: return "literal string";
    case TokenKind::kLiteralChar: return "literal char";
    case TokenKind::kTrue: return "true";
    case TokenKind::kFalse: return "false";

    case TokenKind::kPlus: return "+";
    case TokenKind::kMinus: return "-";
    case TokenKind::kStar: return "*";
    case TokenKind::kSlash: return "/";
    case TokenKind::kPercent: return "%";
    case TokenKind::kStarStar: return "**";
    case TokenKind::kPlusPlus: return "++";
    case TokenKind::kMinusMinus: return "--";
    case TokenKind::kAssign: return "=";
    case TokenKind::kLet: return ":=";
    case TokenKind::kEqEq: return "==";
    case TokenKind::kNeq: return "!=";
    case TokenKind::kLt: return "<";
    case TokenKind::kGt: return ">";
    case TokenKind::kLe: return "<=";
    case TokenKind::kGe: return ">=";
    case TokenKind::kBang: return "!";
    case TokenKind::kAndAnd: return "&&";
    case TokenKind::kPipePipe: return "||";

    case TokenKind::kAmp: return "&";
    case TokenKind::kPipe: return "|";
    case TokenKind::kCaret: return "^";
    case TokenKind::kTilde: return "~";
    case TokenKind::kLtLt: return "<<";
    case TokenKind::kGtGt: return ">>";

    case TokenKind::kPlusEq: return "+=";
    case TokenKind::kMinusEq: return "-=";
    case TokenKind::kStarEq: return "*=";
    case TokenKind::kSlashEq: return "/=";
    case TokenKind::kPercentEq: return "%=";
    case TokenKind::kAmpEq: return "&=";
    case TokenKind::kPipeEq: return "|=";
    case TokenKind::kCaretEq: return "^=";
    case TokenKind::kLtLtEq: return "<<=";
    case TokenKind::kGtGtEq: return ">>=";

    case TokenKind::kArrow: return "->";
    case TokenKind::kColon: return ":";
    case TokenKind::kColonColon: return "::";
    case TokenKind::kSemicolon: return ";";
    case TokenKind::kComma: return ",";
    case TokenKind::kDot: return ".";
    case TokenKind::kDotDot: return "..";
    case TokenKind::kLParen: return "(";
    case TokenKind::kRParen: return ")";
    case TokenKind::kLBrace: return "{";
    case TokenKind::kRBrace: return "}";
    case TokenKind::kLBracket: return "[";
    case TokenKind::kRBracket: return "]";
    case TokenKind::kAt: return "@";
    case TokenKind::kHash: return "#";
    case TokenKind::kDollar: return "$";
    case TokenKind::kQuestion: return "?";

    case TokenKind::kEof: return "eof";
    case TokenKind::kUnknown: return "unknown";
  }
}

}  // namespace lexer

#endif  // FRONTEND_LEXER_TOKEN_TOKEN_KIND_H_
