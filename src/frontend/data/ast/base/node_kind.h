// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODE_KIND_H_
#define FRONTEND_DATA_AST_BASE_NODE_KIND_H_

#include <cstddef>
#include <cstdint>
#include <limits>

#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"

namespace ast {

enum class NodeKind : uint8_t {
  kUnknown = 0,

  kAssignStatement = 1,               // x := 42
  kAttributeStatement = 2,            // #[heap_only]
  kRedirectStatement = 3,             // redirect some_func -> other_func
  kExpressionStatement = 4,           // expression node as statement
  kLiteralExpression = 5,             // 42
  kPathExpression = 6,                // std::io
  kUnaryOperatorExpression = 7,       // !x, -x, x++
  kBinaryOperatorExpression = 8,      // a + b, c * d
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
  kExclusiveRangeExpression = 22,     // 1..<100
  kInclusiveRangeExpression = 23,     // 1..=100
  kReturnExpression = 24,             // ret 0
  kBlockExpression = 25,              // { ... }
  kUnsafeExpression = 26,             // unsafe { ... }
  kFastExpression = 27,               // fast { ... }
  kIfExpression = 28,                 // if cond { ... }
  kLoopExpression = 29,               // loop { ... }
  kWhileExpression = 30,              // while cond { ... }
  kForExpression = 31,                // for i: 1..<100 { ... }
  kMatchExpression = 32,      // match x { a -> break, b -> break, * -> break }
  kClosureExpression = 33,    // (a: i32, b: i32) { ret a + b }
  kFunctionDeclaration = 34,  // fn lucky_number() -> i32 { ret 42 }
  kStructDeclaration = 35,    // struct Vec<T: type, D: u64> { T data[D] }
  kEnumDeclaration = 36,    // enum Result<T: type, E: type> { ok(T), err(E), }
  kTraitDeclaration = 37,   // trait to_string(&this) -> String
  kImplDeclaration = 38,    // impl to_string(&this) { ret format#("n: {}", n) }
  kUnionDeclaration = 39,   // union Bar { i: i32, u: u32 }
  kModuleDeclaration = 40,  // mod foo
};

struct Node {
  NodeKind kind = NodeKind::kUnknown;
  PayloadId payload_id = kInvalidPayloadId;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_KIND_H_
