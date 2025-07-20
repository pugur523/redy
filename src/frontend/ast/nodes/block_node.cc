// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/block_node.h"

#include <string>
#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

BlockNode::BlockNode(const lexer::Token& tok, std::vector<ASTNode>&& stmts)
    : BaseNode(tok), statements(std::move(stmts)) {}

std::string BlockNode::dump() const {
  std::string s;
  s.append("[block_node]\n");
  s.append("[block_node.statements]\n");
  for (const auto& stmt : statements) {
    s.append(dump_ast(stmt));
  }
  return s;
}

}  // namespace ast

