// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_BINARY_OP_NODE_H_
#define FRONTEND_AST_NODES_BINARY_OP_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT BinaryOpNode : BaseNode {
  enum class Operator : uint8_t {
    kUnknown = 0,
    kAdd = 1,                // +
    kSub = 2,                // -
    kMul = 3,                // *
    kDiv = 4,                // /
    kMod = 5,                // %
    kPow = 6,                // **
    kEqual = 7,              // =
    kNotEqual = 8,           // !=
    kLess = 9,               // <
    kGreater = 10,           // >
    kLessEqual = 11,         // <=
    kGreaterEqual = 12,      // >=
    kAnd = 13,               // &&
    kOr = 14,                // ||
    kBitwiseAnd = 15,        // &
    kBitwiseOr = 16,         // |
    kBitwiseXor = 17,        // ^
    kLeftShift = 18,         // <<
    kRightShift = 19,        // >>
    kAssign = 20,            // =
    kAddAssign = 21,         // +=
    kSubAssign = 22,         // -=
    kMulAssign = 23,         // *=
    kDivAssign = 24,         // /=
    kModAssign = 25,         // %=
    kBitwiseAndAssign = 26,  // &=
    kBitwiseOrAssign = 27,   // |=
    kBitwiseXorAssign = 28,  // ^=
    kLeftShiftAssign = 29,   // <<=
    kRightShiftAssign = 30,  // >>=
  };

  Operator op;

  // child nodes are owned by this node (via astnode's unique_ptr)
  AstNode left;
  AstNode right;

  BinaryOpNode(const lexer::Token& tok, Operator o, AstNode&& l, AstNode&& r);

  std::string dump() const override;

  static const char* op_to_string(Operator op) {
    switch (op) {
      case Operator::kUnknown: return "unknown";
      case Operator::kAdd: return "+";
      case Operator::kSub: return "-";
      case Operator::kMul: return "*";
      case Operator::kDiv: return "/";
      case Operator::kMod: return "%";
      case Operator::kPow: return "**";
      case Operator::kEqual: return "=";
      case Operator::kNotEqual: return "!=";
      case Operator::kLess: return "<";
      case Operator::kGreater: return ">";
      case Operator::kLessEqual: return "<=";
      case Operator::kGreaterEqual: return ">=";
      case Operator::kAnd: return "&&";
      case Operator::kOr: return "||";
      case Operator::kBitwiseAnd: return "&";
      case Operator::kBitwiseOr: return "|";
      case Operator::kBitwiseXor: return "^";
      case Operator::kLeftShift: return "<<";
      case Operator::kRightShift: return ">>";
      case Operator::kAssign: return "=";
      case Operator::kAddAssign: return "+=";
      case Operator::kSubAssign: return "-=";
      case Operator::kMulAssign: return "*=";
      case Operator::kDivAssign: return "/=";
      case Operator::kModAssign: return "%=";
      case Operator::kBitwiseAndAssign: return "&=";
      case Operator::kBitwiseOrAssign: return "|=";
      case Operator::kBitwiseXorAssign: return "^=";
      case Operator::kLeftShiftAssign: return "<<=";
      case Operator::kRightShiftAssign: return ">>=";
      default: return "invalid";
    }
  }
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_BINARY_OP_NODE_H_
