// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_IF_NODE_H_
#define FRONTEND_AST_NODES_IF_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT IfNode : BaseNode {
  ASTNode condition;
  ASTNode then_branch;
  std::optional<ASTNode> else_branch;  // else clause is optional

  IfNode(const lexer::Token& tok,
         ASTNode&& cond,
         ASTNode&& then_stmt,
         std::optional<ASTNode>&& else_stmt = std::nullopt);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_IF_NODE_H_
