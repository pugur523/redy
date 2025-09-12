// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/resolver/resolver.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/check.h"
#include "frontend/base/string/string_interner.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/data/hir/payload/common.h"
#include "frontend/processor/resolver/symbol/symbol_table.h"
#include "unicode/utf8/file_manager.h"

namespace resolver {

void Resolver::init(base::StringInterner* interner,
                    std::unique_ptr<ast::Context> ast_context) {
  DCHECK_EQ(status_, Status::kNotInitialized);

  interner_ = interner;
  ast_ctx_ = std::move(ast_context);
  hir_ctx_ = hir::Context::create();
  value_table_ = std::make_unique<SymbolTable>(interner_);
  type_table_ = std::make_unique<SymbolTable>(interner_);
  module_table_ = std::make_unique<SymbolTable>(interner_);

  DCHECK(interner_);
  DCHECK(ast_ctx_);
  DCHECK(hir_ctx_);
  DCHECK(value_table_);
  DCHECK(type_table_);
  DCHECK(module_table_);

  status_ = Status::kReadyToAnalyze;
}

void Resolver::analyze() {
  DCHECK_EQ(status_, Status::kReadyToAnalyze);

  register_root_declarations();

  lower_all();
}

void Resolver::register_root_declarations() {
  const auto& n = nodes();
  for (ast::NodeId i = 0; i < n.size(); ++i) {
    const ast::Node& node = n[i];

    switch (node.kind) {
      using Kind = ast::NodeKind;
      case Kind::kFunctionDeclaration: register_function(node, i); break;
      case Kind::kStructDeclaration: register_struct(node, i); break;
      case Kind::kEnumDeclaration: register_enum(node, i); break;
      case Kind::kTraitDeclaration: register_trait(node, i); break;
      case Kind::kImplDeclaration: register_impl(node, i); break;
      case Kind::kUnionDeclaration: register_union(node, i); break;
      case Kind::kModuleDeclaration: register_module(node, i); break;
      case Kind::kRedirectDeclaration: register_redirect(node, i); break;

      default: break;
    }
  }
}

void Resolver::lower_all() {
  const auto& n = nodes();
  for (ast::NodeId i = 0; i < n.size(); ++i) {  // NOLINT
    const ast::Node& node = n[i];

    switch (node.kind) {
      using Kind = ast::NodeKind;
      case Kind::kAssignStatement: break;
      case Kind::kAttributeStatement: break;
      case Kind::kUseStatement: break;
      case Kind::kLiteralExpression: break;
      case Kind::kPathExpression: break;
      case Kind::kUnaryExpression: break;
      case Kind::kBinaryExpression: break;
      case Kind::kGroupedExpression: break;
      case Kind::kArrayExpression: break;
      case Kind::kTupleExpression: break;
      case Kind::kIndexExpression: break;
      case Kind::kConstructExpression: break;
      case Kind::kFunctionCallExpression: break;
      case Kind::kMethodCallExpression: break;
      case Kind::kFunctionMacroCallExpression: break;
      case Kind::kMethodMacroCallExpression: break;
      case Kind::kFieldAccessExpression: break;
      case Kind::kAwaitExpression: break;
      case Kind::kContinueExpression: break;
      case Kind::kBreakExpression: break;
      case Kind::kRangeExpression: break;
      case Kind::kReturnExpression: break;
      case Kind::kBlockExpression: break;
      case Kind::kIfExpression: break;
      case Kind::kLoopExpression: break;
      case Kind::kWhileExpression: break;
      case Kind::kForExpression: break;
      case Kind::kMatchExpression: break;
      case Kind::kClosureExpression: break;
      case Kind::kFunctionDeclaration: break;
      case Kind::kStructDeclaration: break;
      case Kind::kEnumDeclaration: break;
      case Kind::kTraitDeclaration: break;
      case Kind::kImplDeclaration: break;
      case Kind::kUnionDeclaration: break;
      case Kind::kModuleDeclaration: break;
      case Kind::kRedirectDeclaration: break;

      default: break;
    }
  }
}

void Resolver::register_function(const ast::Node& node, ast::NodeId /* id */) {
  DCHECK_EQ(node.kind, ast::NodeKind::kFunctionDeclaration);

  const auto& payload =
      ast_ctx_->arena<ast::FunctionDeclarationPayload>()[node.payload_id];

  const auto& path =
      ast_ctx_->arena<ast::PathExpressionPayload>()[payload.name.id];
  const auto& ids = path.path_parts_range;
  const auto& ident = ast_ctx_->arena<ast::IdentifierPayload>()[ids.end()];
  value_table_->declare(ident.id, {});
}

}  // namespace resolver
