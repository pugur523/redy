// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/identifier_node.h"

#include <string>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

IdentifierNode::IdentifierNode(const lexer::Token& tok, std::string_view n)
    : BaseNode(tok), name(n) {}

std::string IdentifierNode::dump() const {
  std::string s;
  s.append("[identifier_node]\n");
  s.append("name = ");
  s.append(name);
  s.append("\n");
  return s;
}

}  // namespace ast

