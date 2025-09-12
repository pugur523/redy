// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_PAYLOAD_STATEMENT_H_
#define FRONTEND_DATA_HIR_PAYLOAD_STATEMENT_H_

#include <cstdint>

#include "frontend/base/string/string_interner.h"
#include "frontend/data/ast/payload/data.h"
#include "frontend/data/hir/base/node_id.h"
#include "frontend/data/hir/base/type_id.h"
#include "frontend/data/hir/payload/common.h"
#include "frontend/data/hir/payload/data.h"

namespace hir {

// normal statements

struct AssignStatementPayload {
  PayloadId<ResolvedPathExpressionPayload> target_variable;
  TypeId target_type;
  NodeId value_expression = kInvalidNodeId;
  StorageAttributeData storage_attribute;
};

struct AttributeStatementPayload {
  PayloadRange<AttributeUsePayload> attributes_range;
};

// declaration statements

struct FunctionDeclarationPayload {
  PayloadId<FunctionSignaturePayload> signature;
  NodeId body = kInvalidNodeId;
};

struct StructDeclarationPayload {
  PayloadRange<FieldPayload> fields_range;
  StorageAttributeData attribute;
};

struct EnumerationDeclarationPayload {
  PayloadRange<EnumVariantPayload> variants_range;
  StorageAttributeData attribute;
};

struct TraitDeclarationPayload {
  PayloadRange<FunctionDeclarationPayload> trait_functions_range;
  StorageAttributeData attribute;
};

struct UnionDeclarationPayload {
  PayloadRange<FieldPayload> fields_range;
  StorageAttributeData attribute;
};

struct ModuleDeclarationPayload {
  NodeRange items_range;
  StorageAttributeData attribute;
};

struct GlobalVariableDeclarationPayload {
  TypeId type_id;
  NodeId init = kInvalidNodeId;
  StorageAttributeData attribute;
};

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_PAYLOAD_STATEMENT_H_
