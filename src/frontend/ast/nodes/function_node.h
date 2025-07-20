// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_FUNCTION_NODE_H_
#define FRONTEND_AST_NODES_FUNCTION_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT FunctionNode : BaseNode {
  std::string name;
  ASTNode parameters;   // Owned by this node (expected to be ParameterListNode)
  ASTNode return_type;  // Owned by this node (expected to be TypeNode)
  ASTNode body;         // Owned by this node (expected to be BlockNode)

  FunctionNode(const lexer::Token& tok,
               std::string_view n,
               ASTNode&& params,    // Expects ParameterListNode
               ASTNode&& ret_type,  // Expects TypeNode
               ASTNode&& b);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_FUNCTION_NODE_H_
