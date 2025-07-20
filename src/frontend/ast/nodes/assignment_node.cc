// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/assignment_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

AssignmentNode::AssignmentNode(const lexer::Token& tok,
                               ASTNode&& t,
                               ASTNode&& v,
                               BinaryOpNode::Operator o)
    : BaseNode(tok), target(std::move(t)), value(std::move(v)), op(o) {}

std::string AssignmentNode::dump() const {
  std::string s;
  s.append("[assignment_node]\n");
  s.append(dump_ast(target));
  s.append(dump_ast(value));
  s.append("op = ");
  s.append(BinaryOpNode::op_to_string(op));
  s.append("\n");

  return s;
}

}  // namespace ast
