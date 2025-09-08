// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/resolver/resolver.h"

#include <memory>
#include <utility>
#include <vector>

#include "core/check.h"
#include "frontend/data/ast/base/node.h"
#include "frontend/data/ast/payload/statement.h"
#include "frontend/processor/resolver/symbol/symbol_table.h"
#include "unicode/utf8/file_manager.h"

namespace resolver {

void Resolver::init(std::unique_ptr<ast::Context> ast_context,
                    unicode::Utf8FileManager* manager,
                    unicode::Utf8FileId file_id) {
  DCHECK_EQ(status_, Status::kNotInitialized);

  ast_ctx_ = std::move(ast_context);
  hir_ctx_ = hir::Context::create();
  symbol_table_ = std::make_unique<SymbolTable>();
  manager_ = manager;
  file_id_ = file_id;

  DCHECK(ast_ctx_);
  DCHECK(hir_ctx_);
  DCHECK(symbol_table_);
  DCHECK(manager_);
  DCHECK_NE(file_id_, unicode::kInvalidFileId);

  status_ = Status::kReadyToAnalyze;
}

void Resolver::analyze() {
  DCHECK_EQ(status_, Status::kReadyToAnalyze);

  enter_scope();

  lower_all();

  exit_scope();
}

void Resolver::lower_all() {
  const auto& n = nodes();
  for (ast::NodeId i = 0; i < n.size(); ++i) {
    const ast::Node& node = n[i];

    switch (node.kind) {
      using Kind = ast::NodeKind;
      case Kind::kAssignStatement: break;
      case Kind::kAttributeStatement: break;
      case Kind::kExpressionStatement: break;
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
      case Kind::kUnsafeExpression: break;
      case Kind::kFastExpression: break;
      case Kind::kIfExpression: break;
      case Kind::kLoopExpression: break;
      case Kind::kWhileExpression: break;
      case Kind::kForExpression: break;
      case Kind::kMatchExpression: break;
      case Kind::kClosureExpression: break;
      case Kind::kFunctionDeclaration: lower_function(node, i); break;
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

void Resolver::lower_function(const ast::Node& node, ast::NodeId /* id */) {
  DCHECK_EQ(node.kind, ast::NodeKind::kFunctionDeclaration);

  const auto& payload =
      ast_ctx_->arena<ast::FunctionDeclarationPayload>()[node.payload_id];

  const auto& path =
      ast_ctx_->arena<ast::PathExpressionPayload>()[payload.name.id];
  const auto& ids = path.path_parts_range;
  std::string_view func_name;
  const auto& ident = ast_ctx_->arena<ast::IdentifierPayload>()[ids.end()];
  func_name = lexeme(ident.lexeme_range);
}

}  // namespace resolver
