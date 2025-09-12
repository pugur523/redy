// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_
#define FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_

#include <memory>
#include <vector>

#include "frontend/data/ast/context.h"
#include "frontend/data/hir/context.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/processor/resolver/base/resolver_export.h"
#include "frontend/processor/resolver/symbol/symbol_table.h"
#include "unicode/utf8/file_manager.h"

namespace base {
class StringInterner;
}

namespace resolver {

class RESOLVER_EXPORT Resolver {
 public:
  enum class Status : uint8_t {
    kNotInitialized = 0,
    kReadyToAnalyze = 1,
    kErrorOccured = 2,
    kAnalyzeCompleted = 3,
  };

  Resolver() = default;
  ~Resolver() = default;

  Resolver(const Resolver&) = delete;
  Resolver& operator=(const Resolver&) = delete;

  Resolver(Resolver&&) noexcept = default;
  Resolver& operator=(Resolver&&) noexcept = default;

  void init(base::StringInterner* interner,
            std::unique_ptr<ast::Context> ast_context);

  void analyze();

 private:
  void lower_all();

  void register_root_declarations();
  void register_function(const ast::Node& node, ast::NodeId id);
  void register_struct(const ast::Node& node, ast::NodeId id);
  void register_enum(const ast::Node& node, ast::NodeId id);
  void register_trait(const ast::Node& node, ast::NodeId id);
  void register_impl(const ast::Node& node, ast::NodeId id);
  void register_union(const ast::Node& node, ast::NodeId id);
  void register_module(const ast::Node& node, ast::NodeId id);
  void register_redirect(const ast::Node& node, ast::NodeId id);

  inline const std::vector<ast::Node>& nodes() {
    return ast_ctx_->arena<ast::Node>().buffer();
  }

  std::unique_ptr<ast::Context> ast_ctx_ = nullptr;
  std::unique_ptr<hir::Context> hir_ctx_ = nullptr;
  std::unique_ptr<SymbolTable> value_table_ = nullptr;
  std::unique_ptr<SymbolTable> type_table_ = nullptr;
  std::unique_ptr<SymbolTable> module_table_ = nullptr;
  std::vector<diagnostic::DiagnosticEntry> errors_;
  base::StringInterner* interner_ = nullptr;
  Status status_ = Status::kNotInitialized;
};

}  // namespace resolver

#endif  // FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_
