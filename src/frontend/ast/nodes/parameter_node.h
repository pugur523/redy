// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_PARAMETER_NODE_H_
#define FRONTEND_AST_NODES_PARAMETER_NODE_H_

#include <string>
#include <string_view>

#include "frontend/ast/base/base_node.h"

namespace ast {

// represents a single parameter in a function definition
struct AST_EXPORT ParameterNode : BaseNode {
  std::string_view name;
  AstNode type;  // owned by this node (expected to be TypeNode)

  ParameterNode(const lexer::Token& tok, std::string_view n, AstNode&& t);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_PARAMETER_NODE_H_
