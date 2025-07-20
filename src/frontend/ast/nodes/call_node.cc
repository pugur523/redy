// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/call_node.h"

#include <string>
#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

CallNode::CallNode(const lexer::Token& tok,
                   ASTNode&& c,
                   std::vector<ASTNode>&& args)
    : BaseNode(tok), callee(std::move(c)), arguments(std::move(args)) {}

std::string CallNode::dump() const {
  std::string s;
  s.append("[call_node]\n");
  s.append("callee = ");
  s.append(dump_ast(callee));
  s.append("\n");
  s.append("[call_node.arguments]\n");
  for (const auto& arg : arguments) {
    s.append(dump_ast(arg));
  }
  return s;
}

}  // namespace ast

