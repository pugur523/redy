// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_EXPRESSION_STATEMENT_NODE_H_
#define FRONTEND_AST_NODES_EXPRESSION_STATEMENT_NODE_H_

#include <string>

#include "frontend/ast/base/base_node.h"

namespace ast {

struct AST_EXPORT ExpressionStatementNode : BaseNode {
  ASTNode expression;  // Owned by this node

  ExpressionStatementNode(const lexer::Token& tok, ASTNode&& expr);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_EXPRESSION_STATEMENT_NODE_H_
