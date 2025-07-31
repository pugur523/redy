// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_RETURN_NODE_H_
#define FRONTEND_AST_NODES_RETURN_NODE_H_

#include <optional>
#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT ReturnNode : BaseNode {
  std::optional<AstNode> value;  // owned by this node

  explicit ReturnNode(const lexer::Token& tok,
                      std::optional<AstNode>&& val = std::nullopt);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_RETURN_NODE_H_
