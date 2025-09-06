// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_RESOLVER_SYMBOL_SYMBOL_TABLE_H_
#define FRONTEND_PROCESSOR_RESOLVER_SYMBOL_SYMBOL_TABLE_H_

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "frontend/base/string/string_interner.h"
#include "frontend/data/hir/base/id.h"
#include "frontend/processor/resolver/base/resolver_export.h"

namespace resolver {

class RESOLVER_EXPORT SymbolTable {
 public:
  SymbolTable();
  ~SymbolTable();

  SymbolTable(const SymbolTable&) = delete;
  SymbolTable& operator=(const SymbolTable&) = delete;

  SymbolTable(SymbolTable&&) noexcept = default;
  SymbolTable& operator=(SymbolTable&&) noexcept = default;

  inline void push_scope() { scope_markers_.push_back(symbols_.size()); }

  void pop_scope();

  // declare a symbol in current scope
  // if shadowing, old top is kept in prev chain
  void declare(base::StringId name, hir::HirId target);

  hir::HirId resolve(base::StringId id) const;

  // declare by string
  inline void declare_by_name(std::string_view name_sv, hir::HirId target) {
    declare(interner_.intern(name_sv), target);
  }

  inline hir::HirId resolve_by_name(std::string_view name_sv) const {
    return resolve(interner_.lookup(name_sv));
  }

  // debug utility
  // std::string_view lookup_string(base::StringId id) const {
  //   return interner_.lookup(id);
  // }

 private:
  inline void ensure_top_size(base::StringId name) {
    if (name >= top_of_name_.size()) {
      top_of_name_.resize(name + 1, -1);
    }
  }

  struct SymbolEntry {
    base::StringId name;
    hir::HirId target;
    int32_t prev;  // index into symbols vector (-1 == none)
  };

  base::StringInterner interner_;
  std::vector<SymbolEntry> symbols_;
  std::vector<int32_t> top_of_name_;
  std::vector<std::size_t> scope_markers_;
};

}  // namespace resolver

#endif  // FRONTEND_PROCESSOR_RESOLVER_SYMBOL_SYMBOL_TABLE_H_
