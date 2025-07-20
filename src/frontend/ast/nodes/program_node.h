// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_PROGRAM_NODE_H_
#define FRONTEND_AST_NODES_PROGRAM_NODE_H_

#include <string>
#include <vector>

#include "frontend/ast/base/base_node.h"

namespace ast {

// Program (root node)
struct AST_EXPORT ProgramNode : BaseNode {
  std::vector<ASTNode> statements;

  explicit ProgramNode(const lexer::Token& tok, std::vector<ASTNode>&& stmts);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_PROGRAM_NODE_H_
