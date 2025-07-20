// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/type_node.h"

#include <string>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

TypeNode::TypeNode(const lexer::Token& tok, std::string_view name)
    : BaseNode(tok), type_name(name) {}

std::string TypeNode::dump() const {
  std::string s;
  s.append("[type_node]\n");
  s.append("type = ");
  s.append(type_name);
  s.append("\n");
  return s;
}

}  // namespace ast

