// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_BLOCK_NODE_H_
#define FRONTEND_AST_NODES_BLOCK_NODE_H_

#include <string>
#include <vector>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT BlockNode : BaseNode {
  std::vector<AstNode> statements;  // statements are owned by this node

  explicit BlockNode(const lexer::Token& tok, std::vector<AstNode>&& stmts);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_BLOCK_NODE_H_
