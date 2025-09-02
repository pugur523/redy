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
  kExpressionStatement = 3,           // expression node as statement
  kLiteralExpression = 4,             // 42
  kPathExpression = 5,                // std::io
  kUnaryOperatorExpression = 6,       // !x, -x, x++
  kBinaryOperatorExpression = 7,      // a + b, c * d
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
  kExclusiveRangeExpression = 21,     // 1..<100
  kInclusiveRangeExpression = 22,     // 1..=100
  kReturnExpression = 23,             // ret 0
  kBlockExpression = 24,              // { ... }
  kUnsafeExpression = 25,             // unsafe { ... }
  kFastExpression = 26,               // fast { ... }
  kIfExpression = 27,                 // if cond { ... }
  kLoopExpression = 28,               // loop { ... }
  kWhileExpression = 29,              // while cond { ... }
  kForExpression = 30,                // for i: 1..<100 { ... }
  kMatchExpression = 31,      // match x { a -> break, b -> break, * -> break }
  kClosureExpression = 32,    // (a: i32, b: i32) { ret a + b }
  kFunctionDeclaration = 33,  // fn lucky_number() -> i32 { ret 42 }
  kStructDeclaration = 34,    // struct Vec<T: type, D: u64> { T data[D] }
  kEnumDeclaration = 35,    // enum Result<T: type, E: type> { ok(T), err(E), }
  kTraitDeclaration = 36,   // trait to_string(&this) -> String
  kImplDeclaration = 37,    // impl to_string(&this) { ret format#("n: {}", n) }
  kUnionDeclaration = 38,   // union Bar { i: i32, u: u32 }
  kModuleDeclaration = 39,  // mod foo { ... }
  kRedirectDeclaration = 40,  // redirect some_func -> other_func
};

struct Node {
  NodeKind kind = NodeKind::kUnknown;
  PayloadId payload_id = kInvalidPayloadId;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_KIND_H_
