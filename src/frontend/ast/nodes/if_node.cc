// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/if_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

IfNode::IfNode(const lexer::Token& tok,
               AstNode&& cond,
               AstNode&& then_stmt,
               std::optional<AstNode>&& else_stmt)
    : BaseNode(tok),
      condition(std::move(cond)),
      then_branch(std::move(then_stmt)),
      else_branch(std::move(else_stmt)) {}

std::string IfNode::dump() const {
  std::string s;
  s.append("[if_node]\n");
  s.append(dump_ast(condition));
  s.append(dump_ast(then_branch));
  if (else_branch.has_value()) {
    s.append(dump_ast(*else_branch));
  }
  return s;
}

}  // namespace ast

