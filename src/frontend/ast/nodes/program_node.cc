// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/program_node.h"

#include <string>
#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

ProgramNode::ProgramNode(const lexer::Token& tok, std::vector<ASTNode>&& stmts)
    : BaseNode(tok), statements(std::move(stmts)) {}

std::string ProgramNode::dump() const {
  std::string s;
  s.reserve(statements.size() * 128);  // Pre-allocate memory for efficiency

  s.append("[program_node]\n");
  s.append("[program_node.statements]\n");
  for (const auto& stmt : statements) {
    s.append(dump_ast(stmt));
  }
  return s;
}

}  // namespace ast
