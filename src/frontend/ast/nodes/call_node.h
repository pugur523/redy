// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_CALL_NODE_H_
#define FRONTEND_AST_NODES_CALL_NODE_H_

#include <string>
#include <vector>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT CallNode : BaseNode {
  AstNode callee;
  std::vector<AstNode> arguments;

  CallNode(const lexer::Token& tok, AstNode&& c, std::vector<AstNode>&& args);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_CALL_NODE_H_
