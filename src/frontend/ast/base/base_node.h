// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_AST_BASE_BASE_NODE_H_
#define FRONTEND_AST_BASE_BASE_NODE_H_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "frontend/ast/base/ast_export.h"
#include "frontend/lexer/token/token.h"

namespace ast {

// Forward declarations for all AST nodes
struct ProgramNode;
struct IdentifierNode;
struct LiteralNode;
struct BinaryOpNode;
struct UnaryOpNode;
struct CallNode;
struct BlockNode;
struct IfNode;
struct WhileNode;
struct ForNode;
struct FunctionNode;
struct VariableDeclarationNode;
struct AssignmentNode;
struct ReturnNode;
struct ExpressionStatementNode;
struct ParameterNode;
struct ParameterListNode;
struct TypeNode;

// AST node variant type
// This allows a single ASTNode to hold any type of concrete node.
using ASTNode = std::variant<std::unique_ptr<ProgramNode>,
                             std::unique_ptr<IdentifierNode>,
                             std::unique_ptr<LiteralNode>,
                             std::unique_ptr<BinaryOpNode>,
                             std::unique_ptr<UnaryOpNode>,
                             std::unique_ptr<CallNode>,
                             std::unique_ptr<BlockNode>,
                             std::unique_ptr<IfNode>,
                             std::unique_ptr<WhileNode>,
                             std::unique_ptr<ForNode>,
                             std::unique_ptr<FunctionNode>,
                             std::unique_ptr<VariableDeclarationNode>,
                             std::unique_ptr<AssignmentNode>,
                             std::unique_ptr<ReturnNode>,
                             std::unique_ptr<ExpressionStatementNode>,
                             std::unique_ptr<ParameterNode>,
                             std::unique_ptr<ParameterListNode>,
                             std::unique_ptr<TypeNode>>;

// Base node interface for common operations
struct AST_EXPORT BaseNode {
  // Source token for error reporting.
  const lexer::Token& token;

  explicit BaseNode(const lexer::Token& token) : token(token) {}
  virtual ~BaseNode() = default;

  // Pure virtual function to get a string representation of the node for
  // debugging/dumping.
  virtual std::string dump() const = 0;
};

}  // namespace ast

#endif  // FRONTEND_AST_BASE_BASE_NODE_H_
