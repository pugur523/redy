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

namespace resolver {

Resolver::Resolver(std::unique_ptr<ast::Context> context)
    : context_(std::move(context)) {
  DCHECK(context_);
}

Resolver::~Resolver() = default;

void Resolver::resolve() {
  const std::vector<ast::Node>& nodes = context_->arena<ast::Node>().buffer();

  for (const ast::Node& node : nodes) {
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

}  // namespace resolver
