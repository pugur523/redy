// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_ASSIGNMENT_NODE_H_
#define FRONTEND_AST_NODES_ASSIGNMENT_NODE_H_

#include <string>

#include "frontend/ast/nodes/binary_op_node.h"

namespace ast {

struct AST_EXPORT AssignmentNode : BaseNode {
  ASTNode target;             // Owned by this node
  ASTNode value;              // Owned by this node
  BinaryOpNode::Operator op;  // To distinguish between =, +=, -=, etc.

  AssignmentNode(const lexer::Token& tok,
                 ASTNode&& t,
                 ASTNode&& v,
                 BinaryOpNode::Operator o = BinaryOpNode::Operator::kAssign);

  std::string dump() const override;
};

}  // namespace ast

#endif  // FRONTEND_AST_NODES_ASSIGNMENT_NODE_H_
