// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/unary_op_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

UnaryOpNode::UnaryOpNode(const lexer::Token& tok,
                         Operator o,
                         AstNode&& operand_node)
    : BaseNode(tok), op(o), operand(std::move(operand_node)) {}

std::string UnaryOpNode::dump() const {
  std::string s;
  s.append("[unary_op_node]\n");
  s.append("op = ");
  s.append(op_to_string(op));
  s.append("\n");
  s.append(dump_ast(operand));
  return s;
}

}  // namespace ast

