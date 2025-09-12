// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_PAYLOAD_DATA_H_
#define FRONTEND_DATA_HIR_PAYLOAD_DATA_H_

#include <cstddef>
#include <cstdint>

#include "core/base/source_range.h"
#include "frontend/base/keyword/type.h"
#include "frontend/base/string/string_id.h"
#include "frontend/data/hir/base/node_id.h"
#include "frontend/data/hir/base/type_id.h"
#include "frontend/data/hir/payload/common.h"

namespace hir {

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
    kFhir = 1 << 8,         // 256
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

struct FunctionSignaturePayload {
  PayloadRange<ParameterPayload> params_range;
  TypeId return_type;
  StorageAttributeData attribute;
};

struct AttributeUsePayload {
  NodeId callee;
  NodeRange args_range;
};

struct CapturePayload {
  NodeId capture_name;
  TypeId type;
};

struct FieldPayload {
  NodeId field_name;
  TypeId type;
};

struct ParameterPayload {
  NodeId param_name;
  TypeId type;
};

struct EnumVariantPayload {
  NodeId variant_name;
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
    TypeRange types;

    Data() {}
    ~Data() {}
  } data;

  explicit EnumVariantPayload(NodeId name)
      : variant_name(name), type(VariantType::kEmpty) {}

  explicit EnumVariantPayload(NodeId name, NodeId int_expr)
      : variant_name(name), type(VariantType::kInteger) {
    data.int_expr = int_expr;
  }

  explicit EnumVariantPayload(NodeId name, PayloadRange<FieldPayload> fields)
      : variant_name(name), type(VariantType::kStructLike) {
    data.fields = fields;
  }

  explicit EnumVariantPayload(NodeId name, TypeRange types)
      : variant_name(name), type(VariantType::kTupleLike) {
    data.types = types;
  }
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

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_PAYLOAD_DATA_H_

