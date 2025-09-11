// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODE_H_
#define FRONTEND_DATA_AST_BASE_NODE_H_

#include <cstddef>
#include <cstdint>

#include "frontend/base/data/payload_util.h"
#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"

namespace ast {

enum class NodeKind : uint8_t {
  kUnknown = 0,

  kAssignStatement = 1,      // x := 42
  kAttributeStatement = 2,   // #[heap_only]
  kExpressionStatement = 3,  // expression node as statement
  kUseStatement = 4,         // use some_module::some_function

  kLiteralExpression = 5,             // 42
  kPathExpression = 6,                // std::io
  kUnaryExpression = 7,               // !x, -x, x++
  kBinaryExpression = 8,              // a + b, c * d
  kGroupedExpression = 9,             // (a + b)
  kArrayExpression = 10,              // [4, 2, 5, 7]
  kTupleExpression = 11,              // (4, "25", '7')
  kIndexExpression = 12,              // arr[42] tup[1]
  kConstructExpression = 13,          // Vec{0, 2, 3}
  kFunctionCallExpression = 14,       // some_func()
  kMethodCallExpression = 15,         // vec.some_method()
  kFunctionMacroCallExpression = 16,  // some_macro#()
  kMethodMacroCallExpression = 17,    // vec.some_macro#()
  kFieldAccessExpression = 18,        // foo.some_field
  kAwaitExpression = 19,              // some_async_func()->await
  kContinueExpression = 20,           // continue value
  kBreakExpression = 21,              // break value
  kRangeExpression = 22,              // 1..=100 1..<100
  kReturnExpression = 23,             // ret 0
  kBlockExpression = 24,              // { ... }
  kIfExpression = 25,                 // if cond { ... }
  kLoopExpression = 26,               // loop { ... }
  kWhileExpression = 27,              // while cond { ... }
  kForExpression = 28,                // for i: 1..<100 { ... }
  kMatchExpression = 29,    // match x { a -> break, b -> break, * -> break }
  kClosureExpression = 30,  // (a: i32, b: i32) { ret a + b }

  kFunctionDeclaration = 31,  // fn lucky_number() -> i32 { ret 42 }
  kStructDeclaration = 32,    // struct Vec<T: type, D: u64> { T data[D] }
  kEnumDeclaration = 33,    // enum Result<T: type, E: type> { ok(T), err(E), }
  kTraitDeclaration = 34,   // trait to_string(&this) -> String
  kImplDeclaration = 35,    // impl to_string(&this) { ret format#("n: {}", n) }
  kUnionDeclaration = 36,   // union Bar { i: i32, u: u32 }
  kModuleDeclaration = 37,  // mod foo { ... }
  kRedirectDeclaration = 38,  // redirect some_func -> other_func
};

struct Node {
  uint32_t payload_id = base::kInvalidPayloadId;
  NodeKind kind = NodeKind::kUnknown;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_H_
