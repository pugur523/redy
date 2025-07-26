// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_NODES_NODE_UTIL_H_
#define FRONTEND_AST_NODES_NODE_UTIL_H_

#include <memory>
#include <string>
#include <utility>

#include "frontend/ast/nodes/nodes.h"

namespace ast {

// Function to dump the AST, implemented in .cc file
std::string dump_ast(const AstNode& node);

// Helper functions for AST node creation
template <typename NodeType, typename... Args>
inline AstNode make_node(Args&&... args) {
  return std::make_unique<NodeType>(std::forward<Args>(args)...);
}

// Visitor pattern helper for AstNode
template <typename Visitor>
inline auto visit_node(const AstNode& node, Visitor&& visitor) {
  return std::visit(std::forward<Visitor>(visitor), node);
}

template <typename T>
inline const T* node_as(const ast::AstNode& node) {
  return std::get_if<std::unique_ptr<T>>(&node)->get();
}

}  // namespace ast

#endif  // FRONTEND_AST_NODES_NODE_UTIL_H_
