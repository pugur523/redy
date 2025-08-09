// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_CONTEXT_H_
#define FRONTEND_DATA_AST_BASE_CONTEXT_H_

#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

#include "frontend/base/data/arena.h"
#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"

namespace ast {

class Expr;
class Stmt;
class Pattern;
class Type;
class Identifier;

class AST_EXPORT Context {
 public:
  Context() = default;
  ~Context() = default;

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;

  Context(Context&&) = default;
  Context& operator=(Context&&) = default;

  template <typename T>
  base::Arena<T>* arena();

  template <typename T>
  inline NodeId<T> alloc(const T& value) {
    return arena<T>().alloc(value);
  }

  template <typename T>
  inline T& get(NodeId<T> id) {
    return arena<T>()[id];
  }

 private:
  base::Arena<Expr> exprs_;
  base::Arena<Stmt> stmts_;
  base::Arena<Pattern> patterns_;
  base::Arena<Type> types_;
  base::Arena<Identifier> idents_;
};

template <>
inline base::Arena<Expr>* Context::arena<Expr>() {
  return &exprs_;
}

template <>
inline base::Arena<Stmt>* Context::arena<Stmt>() {
  return &stmts_;
}

template <>
inline base::Arena<Pattern>* Context::arena<Pattern>() {
  return &patterns_;
}

template <>
inline base::Arena<Type>* Context::arena<Type>() {
  return &types_;
}

template <>
inline base::Arena<Identifier>* Context::arena<Identifier>() {
  return &idents_;
}

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_CONTEXT_H_
