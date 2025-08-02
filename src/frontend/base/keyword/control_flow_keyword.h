// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_BASE_KEYWORD_CONTROL_FLOW_KEYWORD_H_
#define FRONTEND_BASE_KEYWORD_CONTROL_FLOW_KEYWORD_H_

#include <cstdint>

#include "frontend/base/token/token_kind.h"

namespace base {

enum class ControlFlowKeyword : uint8_t {
  kUnknown = 0,
  kIf = 1,        // if
  kElse = 2,      // else
  kLoop = 3,      // loop
  kWhile = 4,     // while
  kFor = 5,       // for
  kBreak = 6,     // break
  kContinue = 7,  // continue
  kReturn = 8,    // ret
  kMatch = 9,     // match
};

inline ControlFlowKeyword token_kind_to_control_flow_keyword(TokenKind kind) {
  switch (kind) {
    case TokenKind::kIf: return ControlFlowKeyword::kIf;
    case TokenKind::kElse: return ControlFlowKeyword::kElse;
    case TokenKind::kLoop: return ControlFlowKeyword::kLoop;
    case TokenKind::kWhile: return ControlFlowKeyword::kWhile;
    case TokenKind::kFor: return ControlFlowKeyword::kFor;
    case TokenKind::kBreak: return ControlFlowKeyword::kBreak;
    case TokenKind::kContinue: return ControlFlowKeyword::kContinue;
    case TokenKind::kReturn: return ControlFlowKeyword::kReturn;
    case TokenKind::kMatch: return ControlFlowKeyword::kMatch;
    default: return ControlFlowKeyword::kUnknown;
  }
}

inline bool token_kind_is_control_flow_keyword(TokenKind kind) {
  return token_kind_to_control_flow_keyword(kind) !=
         ControlFlowKeyword::kUnknown;
}

inline const char* control_flow_keyword_to_string(ControlFlowKeyword keyword) {
  switch (keyword) {
    case ControlFlowKeyword::kUnknown: return "unknown";
    case ControlFlowKeyword::kIf: return "if";
    case ControlFlowKeyword::kElse: return "else";
    case ControlFlowKeyword::kLoop: return "loop";
    case ControlFlowKeyword::kWhile: return "while";
    case ControlFlowKeyword::kFor: return "for";
    case ControlFlowKeyword::kBreak: return "break";
    case ControlFlowKeyword::kContinue: return "continue";
    case ControlFlowKeyword::kReturn: return "return";
    case ControlFlowKeyword::kMatch: return "match";
    default: return "invalid";
  }
}

}  // namespace base

#endif  // FRONTEND_BASE_KEYWORD_CONTROL_FLOW_KEYWORD_H_
