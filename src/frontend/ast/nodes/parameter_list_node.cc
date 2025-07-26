// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/parameter_list_node.h"

#include <string>
#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

ParameterListNode::ParameterListNode(const lexer::Token& tok,
                                     std::vector<AstNode> params)
    : BaseNode(tok), parameters(std::move(params)) {}

std::string ParameterListNode::dump() const {
  std::string s;

  s.append("[parameter_list_node]\n");
  s.append("[parameter_list_node.parameters]\n");
  for (const auto& param : parameters) {
    s.append(dump_ast(param));
  }
  return s;
}

}  // namespace ast

