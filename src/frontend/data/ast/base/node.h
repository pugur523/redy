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

  kAssignStatement = 1,     // x := 42
  kAttributeStatement = 2,  // #[heap_only]
  kUseStatement = 3,        // use some_module::some_function

  kLiteralExpression = 4,             // 42
  kPathExpression = 5,                // std::io
  kUnaryExpression = 6,               // !x, -x, x++
  kBinaryExpression = 7,              // a + b, c * d
  kGroupedExpression = 8,             // (a + b)
  kArrayExpression = 9,               // [4, 2, 5, 7]
  kTupleExpression = 10,              // (4, "25", '7')
  kIndexExpression = 11,              // arr[42] tup[1]
  kConstructExpression = 12,          // Vec{0, 2, 3}
  kFunctionCallExpression = 13,       // some_func()
  kMethodCallExpression = 14,         // vec.some_method()
  kFunctionMacroCallExpression = 15,  // some_macro#()
  kMethodMacroCallExpression = 16,    // vec.some_macro#()
  kFieldAccessExpression = 17,        // foo.some_field
  kAwaitExpression = 18,              // some_async_func()->await
  kContinueExpression = 19,           // continue value
  kBreakExpression = 20,              // break value
  kRangeExpression = 21,              // 1..=100 1..<100
  kReturnExpression = 22,             // ret 0
  kBlockExpression = 23,              // { ... }
  kIfExpression = 24,                 // if cond { ... }
  kLoopExpression = 25,               // loop { ... }
  kWhileExpression = 26,              // while cond { ... }
  kForExpression = 27,                // for i: 1..<100 { ... }
  kMatchExpression = 28,    // match x { a -> break, b -> break, * -> break }
  kClosureExpression = 29,  // (a: i32, b: i32) { ret a + b }

  kFunctionDeclaration = 30,  // fn lucky_number() -> i32 { ret 42 }
  kStructDeclaration = 31,    // struct Vec<T: type, D: u64> { T data[D] }
  kEnumDeclaration = 32,    // enum Result<T: type, E: type> { ok(T), err(E), }
  kTraitDeclaration = 33,   // trait to_string(&this) -> String
  kImplDeclaration = 34,    // impl to_string(&this) { ret format#("n: {}", n) }
  kUnionDeclaration = 35,   // union Bar { i: i32, u: u32 }
  kModuleDeclaration = 36,  // mod foo { ... }
  kRedirectDeclaration = 37,  // redirect some_func -> other_func
};

struct Node {
  uint32_t payload_id = base::kInvalidPayloadId;
  NodeKind kind = NodeKind::kUnknown;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_H_
