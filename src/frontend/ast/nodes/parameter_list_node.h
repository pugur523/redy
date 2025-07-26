// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_PARAMETER_LIST_NODE_H_
#define FRONTEND_AST_NODES_PARAMETER_LIST_NODE_H_

#include <string>
#include <vector>

#include "frontend/ast/base/base_node.h"

namespace ast {

// Represents the list of parameters for a function
struct AST_EXPORT ParameterListNode : BaseNode {
  // Vector of ParameterNode. Owned by this node.
  std::vector<AstNode> parameters;

  explicit ParameterListNode(const lexer::Token& tok,
                             std::vector<AstNode> params);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_PARAMETER_LIST_NODE_H_
