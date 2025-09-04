// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_AST_PAYLOAD_STATEMENT_H_
#define FRONTEND_DATA_AST_PAYLOAD_STATEMENT_H_

#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/payload/common.h"
#include "frontend/data/ast/payload/data.h"

namespace ast {

// normal statements

struct AssignStatementPayload {
  PayloadId<PathExpressionPayload> target_variable;
  PayloadId<TypeReferencePayload> target_type;
  NodeId value_expression = kInvalidNodeId;
  StorageAttributeData storage_attribute;
};

struct AttributeStatementPayload {
  PayloadRange<AttributeUsePayload> attributes_range;
};

struct ExpressionStatementPayload {
  NodeId expression;
};

// declaration statements

struct FunctionDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  PayloadRange<ParameterPayload> parameters_range;
  PayloadId<TypeReferencePayload> return_type;
  PayloadId<BlockExpressionPayload> body;
  StorageAttributeData storage_attribute;
};

struct StructDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  PayloadRange<FieldPayload> fields_range;
  StorageAttributeData storage_attribute;
};

struct EnumerationDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  PayloadRange<EnumVariantPayload> variants_range;
  StorageAttributeData storage_attribute;
};

struct TraitDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  NodeRange function_declare_range;
  StorageAttributeData storage_attribute;
};

struct ImplementationDeclarationPayload {
  PayloadId<PathExpressionPayload> target_name;
  PayloadId<PathExpressionPayload> trait_name;
  NodeRange function_definition_range;
  StorageAttributeData storage_attribute;
};

struct UnionDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  PayloadRange<FieldPayload> fields_range;
  StorageAttributeData storage_attribute;
};

struct ModuleDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  NodeRange module_nodes_range;
  StorageAttributeData storage_attribute;
};

struct RedirectDeclarationPayload {
  PayloadId<PathExpressionPayload> name;
  PayloadId<PathExpressionPayload> target;
  StorageAttributeData storage_attribute;
};

}  // namespace ast

#endif  // FRONTEND_DATA_AST_PAYLOAD_STATEMENT_H_
