// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/literal_node.h"

#include <string>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

LiteralNode::LiteralNode(const lexer::Token& tok, Type t, std::string_view v)
    : BaseNode(tok), type(t), value(v) {}

LiteralNode::LiteralNode(const lexer::Token& tok, bool val)
    : BaseNode(tok), type(Type::kBoolean), value(val ? "true" : "false") {}

std::string LiteralNode::dump() const {
  std::string s;
  s.append("[literal_node]\n");
  s.append("type = ");
  s.append(LiteralNode::type_to_string(type));
  s.append("\n");
  s.append("value = ");
  s.append(value);
  s.append("\n");
  return s;
}

}  // namespace ast

