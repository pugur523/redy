// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_PAYLOAD_DATA_H_
#define FRONTEND_DATA_AST_PAYLOAD_DATA_H_

#include <cstddef>
#include <cstdint>

#include "core/base/source_range.h"
#include "frontend/base/keyword/type.h"
#include "frontend/base/string/string_id.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/common.h"

namespace ast {

struct AttributeUsePayload {
  PayloadId<PathExpressionPayload> callee;
  NodeRange args_range;
};

struct CapturePayload {
  PayloadId<PathExpressionPayload> capture_name;
  PayloadId<TypeReferencePayload> type;
};

struct FieldPayload {
  PayloadId<PathExpressionPayload> field_name;
  PayloadId<TypeReferencePayload> type;
};

struct ParameterPayload {
  PayloadId<PathExpressionPayload> param_name;
  PayloadId<TypeReferencePayload> type;
};

struct EnumVariantPayload {
  PayloadId<PathExpressionPayload> variant_name;
  // type nodes or field nodes or invalid (empty)

  enum class VariantType : uint8_t {
    kEmpty = 0,
    kInteger = 1,
    kStructLike = 2,
    kTupleLike = 3,
  } type;

  union Data {
    NodeId int_expr;
    PayloadRange<FieldPayload> fields;
    PayloadRange<TypeReferencePayload> types;

    Data() {}
    ~Data() {}
  } data;

  explicit EnumVariantPayload(PayloadId<PathExpressionPayload> name)
      : variant_name(name), type(VariantType::kEmpty) {}

  explicit EnumVariantPayload(PayloadId<PathExpressionPayload> name,
                              NodeId int_expr)
      : variant_name(name), type(VariantType::kInteger) {
    data.int_expr = int_expr;
  }

  explicit EnumVariantPayload(PayloadId<PathExpressionPayload> name,
                              PayloadRange<FieldPayload> fields)
      : variant_name(name), type(VariantType::kStructLike) {
    data.fields = fields;
  }

  explicit EnumVariantPayload(PayloadId<PathExpressionPayload> name,
                              PayloadRange<TypeReferencePayload> types)
      : variant_name(name), type(VariantType::kTupleLike) {
    data.types = types;
  }
};

struct TypeReferencePayload {
  base::TypeCategory category = base::TypeCategory::kUnknown;

  union Data {
    base::PrimitiveType primitive;
    PayloadId<PathExpressionPayload> user_defined_name;
    PayloadId<ArrayTypePayload> array_id;

    Data() {}
    ~Data() {}
  } data;

  explicit TypeReferencePayload(base::PrimitiveType type)
      : category(base::TypeCategory::kPrimitive) {
    data.primitive = type;
  }

  explicit TypeReferencePayload(PayloadId<PathExpressionPayload> name)
      : category(base::TypeCategory::kUserDefined) {
    data.user_defined_name = name;
  }

  explicit TypeReferencePayload(PayloadId<ArrayTypePayload> array)
      : category(base::TypeCategory::kArray) {
    data.array_id = array;
  }

  inline bool is_primitive() const {
    return category == base::TypeCategory::kPrimitive;
  }
  inline bool is_user_defined() const {
    return category == base::TypeCategory::kUserDefined;
  }
  inline bool is_array() const {
    return category == base::TypeCategory::kArray;
  }

  inline base::PrimitiveType as_primitive() const {
    DCHECK(is_primitive());
    return data.primitive;
  }

  inline PayloadId<PathExpressionPayload> as_user_defined() const {
    DCHECK(is_user_defined());
    return data.user_defined_name;
  }

  inline PayloadId<ArrayTypePayload> as_array() const {
    DCHECK(is_array());
    return data.array_id;
  }
};

struct ArrayTypePayload {
  PayloadId<TypeReferencePayload> type;
  NodeId array_size_expr = kInvalidNodeId;
};

struct IdentifierPayload {
  // core::SourceRange lexeme_range;
  base::StringId id;
};

struct IfBranchPayload {
  // else branch should have invalid condition expression id
  NodeId condition = kInvalidNodeId;
  PayloadId<BlockExpressionPayload> block;
};

struct MatchArmPayload {
  NodeId pattern = kInvalidNodeId;
  NodeId expression = kInvalidNodeId;
};

// storage attribute should be small bitfield structure,
// so we don't have a dedicated arena, but instead just hold raw data
struct StorageAttributeData {
  using DataType = uint16_t;

  enum class Data : DataType {
    kNone = 0,
    kMutable = 1 << 0,      // 1
    kConstant = 1 << 1,     // 2
    kExtern = 1 << 2,       // 4
    kStatic = 1 << 3,       // 8
    kThreadLocal = 1 << 4,  // 16
    kPublic = 1 << 5,       // 32
    kAsync = 1 << 6,        // 64
    kUnsafe = 1 << 7,       // 128
    kFast = 1 << 8,         // 256
  } data = Data::kNone;

  inline bool has_any() const { return !!*this; }

  inline StorageAttributeData operator|(StorageAttributeData::Data other) {
    return {.data = static_cast<StorageAttributeData::Data>(
                static_cast<DataType>(data) | static_cast<DataType>(other))};
  }

  inline StorageAttributeData operator&(StorageAttributeData::Data other) {
    return {.data = static_cast<StorageAttributeData::Data>(
                static_cast<DataType>(data) & static_cast<DataType>(other))};
  }

  inline StorageAttributeData operator|=(StorageAttributeData::Data other) {
    return *this = *this | other;
  }

  inline StorageAttributeData& operator&=(StorageAttributeData::Data other) {
    return *this = *this & other;
  }

  inline explicit operator bool() const { return data != Data::kNone; }
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_PAYLOAD_DATA_H_
