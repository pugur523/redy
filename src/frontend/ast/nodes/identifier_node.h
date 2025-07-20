// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_IDENTIFIER_NODE_H_
#define FRONTEND_AST_NODES_IDENTIFIER_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

// Expressions
struct AST_EXPORT IdentifierNode : BaseNode {
  std::string name;

  IdentifierNode(const lexer::Token& tok, std::string_view n);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_IDENTIFIER_NODE_H_
