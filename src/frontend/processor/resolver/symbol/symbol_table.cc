// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/resolver/symbol/symbol_table.h"

#include "core/check.h"

namespace resolver {

SymbolTable::SymbolTable() {
  // reserve small to avoid reallocation on small program such as hello world
  symbols_.reserve(128);
  scope_markers_.reserve(32);
  // top_of_name is grown when we see new base::StringId (via ensure_top_size)
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::pop_scope() {
  DCHECK(!scope_markers_.empty());
  const std::size_t marker = scope_markers_.back();
  scope_markers_.pop_back();
  // pop symbols back to marker, fixing top_of_name using prev chain
  while (symbols_.size() > marker) {
    const SymbolEntry& e = symbols_.back();
    // restore top_of_name[name] = prev
    top_of_name_[e.name] = e.prev;
    symbols_.pop_back();
  }
}

void SymbolTable::declare(base::StringId name, hir::HirId target) {
  ensure_top_size(name);
  const int32_t prev = top_of_name_[name];
  const int32_t idx = static_cast<int32_t>(symbols_.size());
  symbols_.emplace_back(name, target, prev);
  top_of_name_[name] = idx;
}

hir::HirId SymbolTable::resolve(base::StringId id) const {
  if (id >= top_of_name_.size()) {
    return hir::kInvalidHirId;
  }
  const int32_t idx = top_of_name_[id];
  if (idx < 0) {
    return hir::kInvalidHirId;
  }
  return symbols_[idx].target;
}

}  // namespace resolver
