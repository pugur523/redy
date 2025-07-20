// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/return_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

ReturnNode::ReturnNode(const lexer::Token& tok, std::optional<ASTNode>&& val)
    : BaseNode(tok), value(std::move(val)) {}

std::string ReturnNode::dump() const {
  std::string s;
  s.append("[return_node]\n");
  if (value.has_value()) {
    s.append(dump_ast(*value));
  }
  return s;
}

}  // namespace ast

