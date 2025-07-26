// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/parameter_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

ParameterNode::ParameterNode(const lexer::Token& tok,
                             std::string_view n,
                             AstNode&& t)
    : BaseNode(tok), name(n), type(std::move(t)) {}

std::string ParameterNode::dump() const {
  std::string s;
  s.append("[parameter_node]\n");
  s.append("name = ");
  s.append(name);
  s.append("\n");
  s.append(dump_ast(type));
  return s;
}

}  // namespace ast

