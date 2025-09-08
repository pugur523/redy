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

  void init(std::unique_ptr<ast::Context> ast_context,
            unicode::Utf8FileManager* manager,
            unicode::Utf8FileId file_id);

  void analyze();

 private:
  void lower_all();
  void lower_function(const ast::Node& node, ast::NodeId id);

  inline void enter_scope() { symbol_table_->push_scope(); }
  inline void exit_scope() { symbol_table_->pop_scope(); }

  inline const std::vector<ast::Node>& nodes() {
    return ast_ctx_->arena<ast::Node>().buffer();
  }

  inline const unicode::Utf8File& file() {
    return manager_->loaded_file(file_id_);
  }

  inline const std::u8string_view lexeme_u8(const core::SourceRange& range) {
    return std::u8string_view(file().line_u8(range.start().line()).data() +
                                  range.start().column() - 1,
                              range.length());
  }

  inline const std::string_view lexeme(const core::SourceRange& range) {
    std::u8string_view lexeme_view = lexeme_u8(range);
    return core::to_string_view(lexeme_view);
  }

  std::unique_ptr<ast::Context> ast_ctx_ = nullptr;
  std::unique_ptr<hir::Context> hir_ctx_ = nullptr;
  std::unique_ptr<SymbolTable> symbol_table_ = nullptr;
  std::vector<diagnostic::DiagnosticEntry> errors_;
  unicode::Utf8FileManager* manager_ = nullptr;
  unicode::Utf8FileId file_id_ = unicode::kInvalidFileId;
  Status status_ = Status::kNotInitialized;
};

}  // namespace resolver

#endif  // FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_
