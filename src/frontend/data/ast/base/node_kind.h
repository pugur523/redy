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

using TokenId = std::size_t;

constexpr const TokenId kInvalidTokenId = std::numeric_limits<TokenId>::max();

struct NodeHeader {
  TokenId token_begin = kInvalidTokenId;
  TokenId token_end = kInvalidTokenId;
};

enum class StatementKind : uint8_t {
  kUnknown = 0,

  kAssign = 1,      // x := 42
  kAttribute = 2,   // #[heap_only]
  kRedirect = 3,    // redirect some_func -> other_func
  kExpression = 4,  // expression node as statement
};

enum class ExpressionKind : uint8_t {
  kUnknown = 0,

  // # expressions without block
  kLiteral = 1,          // 42
  kPath = 2,             // std::io
  kUnaryOperator = 3,    // !x, -x, x++
  kBinaryOperator = 4,   // a + b, c * d
  kGrouped = 5,          // (a + b)
  kArray = 6,            // [4, 2, 5, 7]
  kTuple = 7,            // (4, "25", '7')
  kIndex = 8,            // arr[42] tup[1]
  kConstruct = 9,        // Vec{0, 2, 3}
  kFunctionCall = 10,    // some_func()
  kMethodCall = 11,      // vec.some_method()
  kMacroCall = 12,       // some_macro#()
  kFieldAccess = 13,     // foo.some_field
  kAwait = 14,           // some_async_func()->await
  kContinue = 15,        // continue value
  kBreak = 16,           // break value
  kExclusiveRange = 17,  // 1..<100
  kInclusiveRange = 18,  // 1..=100
  kReturn = 19,          // ret 0

  // # expressions with block
  kBlock = 20,    // { ... }
  kUnsafe = 21,   // unsafe { ... }
  kFast = 22,     // fast { ... }
  kIf = 23,       // if cond { ... }
  kLoop = 24,     // loop { ... }
  kWhile = 25,    // while cond { ... }
  kFor = 26,      // for i: 1..<100 { ... }
  kMatch = 27,    // match x { a -> break, b -> break, * -> break }
  kClosure = 28,  // (a: i32, b: i32) { ret a + b }
};

enum class BlockDeclarationKind : uint8_t {
  kUnknown = 0,

  kFunction = 1,        // fn lucky_number() -> i32 { ret 42 }
  kStruct = 2,          // struct Vec<T: type, D: u64> { T data[D] }
  kEnumeration = 3,     // enum Result<T: type, E: type> { ok(T), err(E), }
  kTrait = 4,           // trait to_string(&this) -> String
  kImplementation = 5,  // impl to_string(&this) { ret format#("n: {}", n) }
  kUnion = 6,           // union Bar { i: i32, u: u32 }
  kModule = 7,          // mod foo
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODE_KIND_H_
