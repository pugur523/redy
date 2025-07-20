// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/nodes/expression_statement_node.h"

#include <string>
#include <utility>

#include "frontend/ast/nodes/node_util.h"

namespace ast {

ExpressionStatementNode::ExpressionStatementNode(const lexer::Token& tok,
                                                 ASTNode&& expr)
    : BaseNode(tok), expression(std::move(expr)) {}

std::string ExpressionStatementNode::dump() const {
  std::string s;
  s.append("[expression_statement_node]\n");
  s.append(dump_ast(expression));
  return s;
}

}  // namespace ast

