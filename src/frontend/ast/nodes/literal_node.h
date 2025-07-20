// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_LITERAL_NODE_H_
#define FRONTEND_AST_NODES_LITERAL_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"
namespace ast {

struct AST_EXPORT LiteralNode : BaseNode {
  enum class Type : uint8_t {
    kUnknown = 0,
    kNumeric = 1,
    kChar = 2,
    kString = 3,
    kBoolean = 4,
  };
  Type type;
  std::string value;

  LiteralNode(const lexer::Token& tok, Type t, std::string_view v);

  // Overload for boolean true/false
  LiteralNode(const lexer::Token& tok, bool val);

  std::string dump() const override;

  static const char* type_to_string(Type type) {
    switch (type) {
      case Type::kUnknown: return "unknown";
      case Type::kNumeric: return "numeric";
      case Type::kChar: return "char";
      case Type::kString: return "string";
      case Type::kBoolean: return "boolean";
    }
  }
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_LITERAL_NODE_H_
