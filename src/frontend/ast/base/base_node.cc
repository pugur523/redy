// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/ast/base/base_node.h"

#include <string>
#include <utility>
#include <vector>

#include "frontend/ast/nodes/node_util.h"
#include "frontend/ast/nodes/nodes.h"

namespace ast {

// Global AST dump function
std::string dump_ast(const ASTNode& node) {
  return visit_node(node, [](auto&& arg) { return arg->dump(); });
}

}  // namespace ast
