// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/variable_declaration_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

VariableDeclarationNode::VariableDeclarationNode(const lexer::Token& tok,
                                                 std::string_view n,
                                                 bool is_mutable,
                                                 std::optional<ASTNode>&& t,
                                                 std::optional<ASTNode>&& init)
    : BaseNode(tok),
      name(n),
      is_mutable(is_mutable),
      type(std::move(t)),
      initializer(std::move(init)) {}

std::string VariableDeclarationNode::dump() const {
  std::string s;
  s.append("[variable_declaration_node]\n");
  s.append("name = ");
  s.append(name);
  s.append("\n");
  s.append("is_mutable = ");
  s.append(is_mutable ? "true" : "false");
  s.append("\n");
  if (type.has_value()) {
    s.append(dump_ast(*type));
  }
  if (initializer.has_value()) {
    s.append(dump_ast(*initializer));
  }
  return s;
}

}  // namespace ast

