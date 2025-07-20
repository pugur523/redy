// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/binary_op_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

BinaryOpNode::BinaryOpNode(const lexer::Token& tok,
                           Operator o,
                           ASTNode&& l,
                           ASTNode&& r)
    : BaseNode(tok), op(o), left(std::move(l)), right(std::move(r)) {}

std::string BinaryOpNode::dump() const {
  std::string s;
  s.append("[binary_op_node]\n");
  s.append("op = ");
  s.append(BinaryOpNode::op_to_string(op));
  s.append("\n");
  s.append(dump_ast(left));
  s.append(dump_ast(right));
  return s;
}

}  // namespace ast

