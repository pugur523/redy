// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/for_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

ForNode::ForNode(const lexer::Token& tok,
                 std::optional<AstNode>&& i,
                 std::optional<AstNode>&& cond,
                 std::optional<AstNode>&& incr,
                 AstNode&& b)
    : BaseNode(tok),
      init(std::move(i)),
      condition(std::move(cond)),
      increment(std::move(incr)),
      body(std::move(b)) {}

std::string ForNode::dump() const {
  std::string s;
  s.append("[for_node]\n");
  if (init.has_value()) {
    s.append(dump_ast(*init));
  }
  if (condition.has_value()) {
    s.append(dump_ast(*condition));
  }
  if (increment.has_value()) {
    s.append(dump_ast(*increment));
  }
  s.append(dump_ast(body));

  return s;
}

}  // namespace ast

