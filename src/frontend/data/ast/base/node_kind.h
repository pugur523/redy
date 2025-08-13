// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_BASE_NODES_H_
#define FRONTEND_DATA_AST_BASE_NODES_H_

#include <cstddef>
#include <cstdint>

#include "frontend/base/keyword/type_keyword.h"
#include "frontend/data/ast/base/ast_export.h"
#include "frontend/data/ast/base/node_id.h"

namespace ast {

using TokenId = std::size_t;
constexpr const TokenId kInvalidTokenId = std::numeric_limits<TokenId>::max();

enum class TypeKind : uint8_t {
  kUnknown = 0,
  kAuto = 1,
  kI8 = 2,
  kI16 = 3,
  kI32 = 4,
  kI64 = 5,
  kI128 = 6,
  kIsize = 7,
  kU8 = 8,
  kU16 = 9,
  kU32 = 10,
  kU64 = 11,
  kU128 = 12,
  kUsize = 13,
  kF32 = 14,
  kF64 = 15,
  kVoid = 16,
  kBool = 17,
  kChar = 18,
  kByte = 19,
  kUserDefined = 20,
};

// inline constexpr const char* type_kind_to_string(TypeKind kind) {
//   switch (kind) {
//     case TypeKind::kUnknown: return "unknown";
//     case TypeKind::kAuto: return "auto";
//     case TypeKind::kI8: return "i8";
//     case TypeKind::kI16: return "i16";
//     case TypeKind::kI32: return "i32";
//     case TypeKind::kI64: return "i64";
//     case TypeKind::kI128: return "i128";
//     case TypeKind::kIsize: return "isize";
//     case TypeKind::kU8: return "u8";
//     case TypeKind::kU16: return "u16";
//     case TypeKind::kU32: return "u32";
//     case TypeKind::kU64: return "u64";
//     case TypeKind::kU128: return "u128";
//     case TypeKind::kF32: return "f32";
//     case TypeKind::kF64: return "f64";
//     case TypeKind::kVoid: return "void";
//     case TypeKind::kBool: return "bool";
//     case TypeKind::kChar: return "char";
//     case TypeKind::kByte: return "byte";
//     case TypeKind::kUserDefined: return "user defined";
//     default: DCHECK(false); return "invalid";
//   }
// }

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
  kAwait = 15,           // await some_async_func()
  kContinue = 16,        // continue
  kBreak = 17,           // break
  kExclusiveRange = 18,  // 1..<100
  kInclusiveRange = 19,  // 1..=100
  kReturn = 20,          // ret 0

  // # expressions with block
  kBlock = 21,       // { ... }
  kConstBlock = 22,  // const { ... }
  kUnsafe = 23,      // unsafe { ... }
  kFast = 24,        // fast { ... }
  kIf = 25,          // if cond { ... }
  kLoop = 26,        // loop { ... }
  kWhile = 27,       // while cond { ... }
  kFor = 28,         // for i: 1..<100 { ... }
  kMatch = 29,       // match x { a -> break, b -> break, * -> break }
  kClosure = 30,     // (a: i32, b: i32) { ret a + b }
};

struct NodeHeader {
  TokenId token_begin = kInvalidTokenId;
  TokenId token_end = kInvalidTokenId;
};

enum class StatementKind : uint8_t {
  kUnknown = 0,

  kAssign = 1,       // x := 42
  kConstAssign = 2,  // const x := 57
  kAttribute = 3,    // #[heap_only]
  kRedirect = 4,     // redirect some_func -> other_func
  kExpression = 5,   // expression node as statement
};

enum class BlockDeclarationKind : uint8_t {
  kUnknown = 0,

  kFunction = 1,     // fn lucky_number() -> i32 { ret 42 }
  kStruct = 2,       // struct Vec<T: type, D: u64> { T data[D] }
  kEnumeration = 3,  // enum Result<T: type, E: type> { ok(T), err(E), }
  kUnion = 4,        // union Bar { i: i32, u: u32 }
  kModule = 5,       // mod foo
};

// struct AssignStmtData {
//   NodeId left_hand_side;
//   NodeId right_hand_side;
// };
//
// struct ConstAssignStmtData {
//   NodeId left_hand_side;
//   NodeId right_hand_side;
// };
//
// struct ExpressionStmtData {
//   NodeId expression;
// };

}  // namespace ast

#endif  // FRONTEND_DATA_AST_BASE_NODES_H_
