// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/function_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

FunctionNode::FunctionNode(const lexer::Token& tok,
                           std::string_view n,
                           ASTNode&& params,
                           ASTNode&& ret_type,
                           ASTNode&& b)
    : BaseNode(tok),
      name(n),
      parameters(std::move(params)),
      return_type(std::move(ret_type)),
      body(std::move(b)) {}

std::string FunctionNode::dump() const {
  std::string s;
  s.append("[function_node]\n");
  s.append("name = ");
  s.append(name);
  s.append("\n");
  s.append(dump_ast(parameters));
  s.append(dump_ast(return_type));
  s.append(dump_ast(body));
  return s;
}

}  // namespace ast

