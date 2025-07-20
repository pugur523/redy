// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_VARIABLE_DECLARATION_NODE_H_
#define FRONTEND_AST_NODES_VARIABLE_DECLARATION_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT VariableDeclarationNode : BaseNode {
  std::string name;
  bool is_mutable;
  std::optional<ASTNode> type;         // Owned by this node
  std::optional<ASTNode> initializer;  // Owned by this node

  VariableDeclarationNode(const lexer::Token& tok,
                          std::string_view n,
                          bool is_mutable = false,
                          std::optional<ASTNode>&& t = std::nullopt,
                          std::optional<ASTNode>&& init = std::nullopt);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_VARIABLE_DECLARATION_NODE_H_
