// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_FOR_NODE_H_
#define FRONTEND_AST_NODES_FOR_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT ForNode : BaseNode {
  std::optional<AstNode> init;       // i := 0
  std::optional<AstNode> condition;  // i < 10
  std::optional<AstNode> increment;  // i++
  AstNode body;

  ForNode(const lexer::Token& tok,
          std::optional<AstNode>&& i,
          std::optional<AstNode>&& cond,
          std::optional<AstNode>&& incr,
          AstNode&& b);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_FOR_NODE_H_
