// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_UNARY_OP_NODE_H_
#define FRONTEND_AST_NODES_UNARY_OP_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT UnaryOpNode : BaseNode {
  enum class Operator : uint8_t {
    kUnknown = 0,
    kNot = 1,         // ! (logical NOT)
    kNegate = 2,      // - (unary minus)
    kBitwiseNot = 3,  // ~ (bitwise NOT)
    kIncrement = 4,   // ++
    kDecrement = 5,   // --
  };

  Operator op;

  // child node is owned by this node (via AstNode's unique_ptr)
  AstNode operand;

  UnaryOpNode(const lexer::Token& tok, Operator o, AstNode&& operand_node);

  std::string dump() const override;

  static const char* op_to_string(Operator op) {
    switch (op) {
      case Operator::kUnknown: return "unknown";
      case Operator::kNot: return "!";
      case Operator::kNegate: return "-";
      case Operator::kBitwiseNot: return "~";
      case Operator::kIncrement: return "++";
      case Operator::kDecrement: return "--";
    }
  }
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_UNARY_OP_NODE_H_
