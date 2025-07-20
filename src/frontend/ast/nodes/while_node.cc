// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/while_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

WhileNode::WhileNode(const lexer::Token& tok, ASTNode&& cond, ASTNode&& b)
    : BaseNode(tok), condition(std::move(cond)), body(std::move(b)) {}

std::string WhileNode::dump() const {
  std::string s;
  s.append("[while_node]\n");
  s.append(dump_ast(condition));
  s.append(dump_ast(body));
  return s;
}

}  // namespace ast

