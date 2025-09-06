// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_PAYLOAD_COMMON_H_
#define FRONTEND_DATA_HIR_PAYLOAD_COMMON_H_

#include "core/base/source_range.h"
#include "frontend/base/data/payload_util.h"
#include "frontend/base/keyword/type.h"
#include "frontend/base/literal/literal.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/data/hir/base/id.h"

namespace hir {

template <typename T>
using PayloadId = base::PayloadId<T>;

template <typename T>
using PayloadRange = base::PayloadRange<T>;

using HirIdRange = PayloadRange<HirId>;
using DefIdRange = PayloadRange<DefId>;
using LocalIdRange = PayloadRange<LocalId>;

// forward declarations
struct ResolvedPathPayload;
struct TypePayload;
struct PatternPayload;
struct StructPatternFieldPayload;
struct ExpressionPayload;
struct StatementPayload;
struct DeclarationPayload;
struct ParameterPayload;
struct FieldPayload;
struct EnumVariantPayload;
struct MatchArmPayload;
struct GenericArgPayload;
struct CapturePayload;

// resolved path after name resolution
struct ResolvedPathPayload {
  DefId def_id;
  PayloadRange<GenericArgPayload> generic_args_range;
  core::SourceRange source_range;
};

// generic arguments (types or const expressions)
struct GenericArgPayload {
  enum class Kind : uint8_t {
    kType = 0,
    kConst = 1,
  } kind;

  union Data {
    PayloadId<TypePayload> type_id;
    // resolved constant expression
    HirId const_expr_id;

    Data() {}
    ~Data() {}
  } data;
};

// types after resolution and desugaring
enum class TypeKind : uint8_t {
  kPrimitive = 0,
  kAdt = 1,  // struct, enum, union
  kReference = 2,
  kPointer = 3,
  kArray = 4,
  kSlice = 5,
  kTuple = 6,
  kFunction = 7,
  kClosure = 8,
  kNever = 9,   // ! type
  kInfer = 10,  // type inference placeholder
  kError = 11,  // error recovery
};

struct TypePayload {
  TypeKind kind;

  union Data {
    base::PrimitiveType primitive;

    struct {
      DefId def_id;
      PayloadRange<GenericArgPayload> generic_args_range;
    } adt;

    struct {
      PayloadId<TypePayload> pointee;
      bool is_mutable;
      // lifetime info would go here
    } reference;

    struct {
      PayloadId<TypePayload> pointee;
      bool is_mutable;
    } pointer;

    struct {
      PayloadId<TypePayload> element_type;
      uint64_t size;  // resolved constant
    } array;

    struct {
      PayloadId<TypePayload> element_type;
    } slice;

    struct {
      PayloadRange<TypePayload> element_types_range;
    } tuple;

    struct {
      PayloadRange<TypePayload> param_types_range;
      PayloadId<TypePayload> return_type;
      bool is_async;
      bool is_unsafe;
    } function;

    struct {
      PayloadRange<LocalId> captures_range;
      PayloadRange<TypePayload> param_types_range;
      PayloadId<TypePayload> return_type;
    } closure;

    Data() {}
    ~Data() {}
  } data;
};

// patterns after desugaring
enum class PatternKind : uint8_t {
  kWild = 0,       // *
  kBinding = 1,    // x, mut x
  kLiteral = 2,    // 42, "hello"
  kRange = 3,      // 1..=10
  kTuple = 4,      // (a, b, c)
  kStruct = 5,     // Point { x, y }
  kEnum = 6,       // Some(x)
  kReference = 7,  // &x, &mut x
  kSlice = 8,      // [a, b, ..]
};

struct PatternPayload {
  PatternKind kind;
  PayloadId<TypePayload> type_id;
  core::SourceRange source_range;

  union Data {
    struct {
      LocalId local_id;
      bool is_mutable;
    } binding;

    struct {
      base::LiteralKind literal_kind;
      core::SourceRange lexeme_range;
    } literal;

    struct {
      PayloadId<PatternPayload> start;
      PayloadId<PatternPayload> end;
      bool is_inclusive;
    } range;

    struct {
      PayloadRange<PatternPayload> elements_range;
    } tuple;

    struct {
      DefId def_id;
      PayloadRange<StructPatternFieldPayload> fields_range;
      bool has_rest;
    } struct_pattern;

    struct {
      DefId enum_id;
      DefId variant_id;
      PayloadRange<PatternPayload> sub_patterns_range;
    } enum_pattern;

    struct {
      PayloadId<PatternPayload> inner;
      bool is_mutable;
    } reference;

    struct {
      PayloadRange<PatternPayload> elements_range;
      // for .. pattern
      PayloadId<PatternPayload> rest;
    } slice;

    Data() {}
    ~Data() {}
  } data;
};

struct StructPatternFieldPayload {
  DefId field_def_id;
  PayloadId<PatternPayload> pattern;
};

// expressions after desugaring and resolution
enum class ExpressionKind : uint8_t {
  kLiteral = 0,
  kPath = 1,
  kUnary = 2,
  kBinary = 3,
  kAssign = 4,
  kArray = 5,
  kTuple = 6,
  kIndex = 7,
  kCall = 8,
  kMethodCall = 9,
  kFieldAccess = 10,
  kCast = 11,
  kBlock = 12,
  kIf = 13,
  kMatch = 14,
  kWhile = 15,  // loop will be desugared to while
  kFor = 16,
  kBreak = 17,
  kContinue = 18,
  kReturn = 19,
  kClosure = 20,
  kAwait = 21,
  kRange = 22,
};

struct ExpressionPayload {
  ExpressionKind kind;
  PayloadId<TypePayload> type_id;
  core::SourceRange source_range;

  union Data {
    struct {
      base::LiteralKind literal_kind;
      core::SourceRange lexeme_range;
    } literal;

    PayloadId<ResolvedPathPayload> path;

    struct {
      base::UnaryOperator op;
      HirId operand;
    } unary;

    struct {
      base::BinaryOperator op;
      HirId lhs;
      HirId rhs;
    } binary;

    struct {
      HirId lhs;
      HirId rhs;
    } assign;

    struct {
      HirIdRange elements_range;
    } array;

    struct {
      HirIdRange elements_range;
    } tuple;

    struct {
      HirId object;
      HirId index;
    } index;

    struct {
      HirId callee;
      HirIdRange args_range;
    } call;

    struct {
      HirId receiver;
      DefId method_def_id;
      PayloadRange<GenericArgPayload> generic_args_range;
      HirIdRange args_range;
    } method_call;

    struct {
      HirId object;
      DefId field_def_id;
    } field_access;

    struct {
      HirId expr;
      PayloadId<TypePayload> target_type;
    } cast;

    struct {
      PayloadRange<StatementPayload> stmts_range;
      // optional trailing expr (kInvalidHirId if none)
      HirId expr;
      bool is_unsafe;
    } block;

    struct {
      HirId condition;
      HirId then_branch;
      // kInvalidHirId if no else
      HirId else_branch;
    } if_expr;

    struct {
      HirId scrutinee;
      PayloadRange<MatchArmPayload> arms_range;
    } match_expr;

    struct {
      HirId condition;
      HirId body;
    } while_expr;

    struct {
      PayloadId<PatternPayload> pattern;
      HirId iterable;
      HirId body;
    } for_expr;

    struct {
      // kInvalidHirId if no value
      HirId value;
      // label info would go here
    } break_expr;

    struct {
      // kInvalidHirId if no value
      HirId value;
    } continue_expr;

    struct {
      // kInvalidHirId if no value
      HirId value;
    } return_expr;

    struct {
      PayloadRange<CapturePayload> captures_range;
      PayloadRange<ParameterPayload> params_range;
      HirId body;
      bool is_async;
    } closure;

    struct {
      HirId expr;
    } await_expr;

    struct {
      // kInvalidHirId if no start
      HirId start;
      // kInvalidHirId if no end
      HirId end;
      bool is_inclusive;
    } range;

    Data() {}
    ~Data() {}
  } data;
};

struct MatchArmPayload {
  PayloadId<PatternPayload> pattern;
  // kInvalidHirId if no guard
  HirId guard;
  HirId body;
};

struct CapturePayload {
  LocalId local_id;
  PayloadId<TypePayload> type_id;
  bool by_ref;
  bool is_mutable;
};

// statements after desugaring
enum class StatementKind : uint8_t {
  kExpression = 0,   // expression wraped
  kLocal = 1,        // binding
  kDeclaration = 2,  // nested declaration
};

struct StatementPayload {
  StatementKind kind;
  core::SourceRange source_range;

  union Data {
    HirId expr;

    struct {
      PayloadId<PatternPayload> pattern;
      // kInvalidPayloadId if no annotation
      PayloadId<TypePayload> type_id;
      // kInvalidHirId if no initializer
      HirId init;
    } local;

    PayloadId<DeclarationPayload> item;

    Data() {}
    ~Data() {}
  } data;
};

enum class VisibilityLevel : uint8_t {
  kPrivate = 0,
  kPublic = 1,
  kPubCrate = 2,
  kPubSuper = 3,
  kPubIn = 4,  // pub(in path)
};

struct VisibilityPayload {
  VisibilityLevel level = VisibilityLevel::kPrivate;
  // for pub(in path)
  PayloadId<ResolvedPathPayload> restricted_path;
};

struct ParameterPayload {
  LocalId local_id;
  PayloadId<PatternPayload> pattern;
  PayloadId<TypePayload> type_id;
};

struct FieldPayload {
  DefId def_id;
  PayloadId<VisibilityPayload> visibility;
  PayloadId<TypePayload> type_id;
  core::SourceRange source_range;
};

struct EnumVariantPayload {
  DefId def_id;
  PayloadId<VisibilityPayload> visibility;

  enum class VariantKind : uint8_t {
    kUnit = 0,     // Variant
    kTuple = 1,    // Variant(T, U)
    kStruct = 2,   // Variant { field: T }
    kInteger = 3,  // Variant = 0
  } kind;

  union Data {
    PayloadRange<TypePayload> tuple_fields_range;
    PayloadRange<FieldPayload> struct_fields_range;

    Data() {}
    ~Data() {}
  } data;

  core::SourceRange source_range;
};

// top-level declarations after resolution
enum class DeclarationKind : uint8_t {
  kFunction = 0,
  kStruct = 1,
  kEnum = 2,
  kUnion = 3,
  kTrait = 4,
  kImpl = 5,
  kModule = 6,
  // TODO: add use token and parser
  // kUse = 7,
  kStatic = 8,
  kConst = 9,
  kTypeAlias = 10,
};

struct DeclarationPayload {
  DefId def_id;
  DeclarationKind kind;
  PayloadId<VisibilityPayload> visibility;
  core::SourceRange source_range;

  union Data {
    struct {
      PayloadRange<ParameterPayload> params_range;
      PayloadId<TypePayload> return_type;
      HirId body;
      bool is_async;
      bool is_unsafe;
      bool is_extern;
    } function;

    struct {
      PayloadRange<FieldPayload> fields_range;
    } struct_item;

    struct {
      PayloadRange<EnumVariantPayload> variants_range;
    } enum_item;

    struct {
      PayloadRange<FieldPayload> fields_range;
    } union_item;

    struct {
      PayloadRange<DeclarationPayload> trait_items_range;
    } trait_item;

    struct {
      PayloadId<TypePayload> self_type;
      // kInvalidPayloadId if inherent impl
      PayloadId<ResolvedPathPayload> trait_path;
      PayloadRange<DeclarationPayload> impl_items_range;
    } impl_item;

    struct {
      PayloadRange<DeclarationPayload> items_range;
    } module;

    struct {
      PayloadId<ResolvedPathPayload> imported_path;
      // use alias info would go here
    } use_item;

    struct {
      PayloadId<TypePayload> type_id;
      // kInvalidHirId for extern static
      HirId init;
      bool is_mutable;
    } static_item;

    struct {
      PayloadId<TypePayload> type_id;
      HirId init;
    } const_item;

    struct {
      PayloadId<TypePayload> aliased_type;
    } type_alias;

    Data() {}
    ~Data() {}
  } data;
};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_PAYLOAD_COMMON_H_
