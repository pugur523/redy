// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DATA_HIR_CONTEXT_H_
#define FRONTEND_DATA_HIR_CONTEXT_H_

#include <cstdint>
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "frontend/base/data/arena.h"
#include "frontend/data/hir/base/hir_export.h"
#include "frontend/data/hir/base/id.h"
#include "frontend/data/hir/base/node.h"
#include "frontend/data/hir/payload/common.h"

namespace hir {

class HIR_EXPORT HirContext {
 public:
  static inline std::unique_ptr<HirContext> create() {
    return std::unique_ptr<HirContext>(new HirContext());
  }

  ~HirContext() = default;

  HirContext(const HirContext&) = delete;
  HirContext& operator=(const HirContext&) = delete;

  HirContext(HirContext&&) noexcept = default;
  HirContext& operator=(HirContext&&) noexcept = default;

  template <typename T>
  inline constexpr base::Arena<T>& arena();

  template <typename T>
  inline HirId alloc(T&& value) {
    return arena<T>().alloc(std::move(value));
  }

  template <typename T>
  inline PayloadId<T> alloc_payload(T&& value) {
    return PayloadId<T>(alloc(std::move(value)));
  }

  template <typename T>
  inline HirId alloc_hir_node(HirNodeKind kind, T&& payload) {
    const PayloadId<T> payload_id = alloc_payload<T>(std::move(payload));
    return alloc(HirNode{
        .payload_id = payload_id.id,
        .kind = kind,
    });
  }

  template <typename T>
  inline T& get(uint32_t id) {
    return arena<T>()[id];
  }

  // id generators
  inline constexpr DefId next_def_id() {
    ++next_def_id_.id;
    return next_def_id_;
  }
  inline constexpr HirId next_hir_id() {
    ++next_hir_id_.id;
    return next_hir_id_;
  }
  inline constexpr LocalId next_local_id() {
    ++next_local_id_.id;
    return next_local_id_;
  }

 private:
  HirContext() = default;

  // id generators
  DefId next_def_id_ = {0};
  HirId next_hir_id_ = {0};
  LocalId next_local_id_ = {0};

  base::Arena<HirNode> hir_nodes_;
  base::Arena<ExpressionPayload> expression_payloads_;
  base::Arena<StatementPayload> statement_payloads_;
  base::Arena<PatternPayload> pattern_payloads_;
  base::Arena<TypePayload> type_payloads_;
  base::Arena<DeclarationPayload> item_payloads_;
  base::Arena<ResolvedPathPayload> resolved_path_payloads_;
  base::Arena<GenericArgPayload> generic_arg_payloads_;
  base::Arena<ParameterPayload> parameter_payloads_;
  base::Arena<FieldPayload> field_payloads_;
  base::Arena<EnumVariantPayload> enum_variant_payloads_;
  base::Arena<MatchArmPayload> match_arm_payloads_;
  base::Arena<CapturePayload> capture_payloads_;
  base::Arena<StructPatternFieldPayload> struct_pattern_field_payloads_;
  base::Arena<VisibilityPayload> visibility_payloads_;

  // raw id arrays for ranges
  base::Arena<HirId> hir_ids_;
  base::Arena<DefId> def_ids_;
  base::Arena<LocalId> local_ids_;

  // resolution maps
  // std::unordered_map<ast::NodeId, DefId> ast_to_def;
  // std::unordered_map<ast::NodeId, HirId> ast_to_hir;
  // std::unordered_map<HirId, PayloadId<TypePayload>> hir_to_type;
};

template <>
inline constexpr base::Arena<HirNode>& HirContext::arena<HirNode>() {
  return hir_nodes_;
}

template <>
inline constexpr base::Arena<ExpressionPayload>&
HirContext::arena<ExpressionPayload>() {
  return expression_payloads_;
}
template <>
inline constexpr base::Arena<StatementPayload>&
HirContext::arena<StatementPayload>() {
  return statement_payloads_;
}
template <>
inline constexpr base::Arena<PatternPayload>&
HirContext::arena<PatternPayload>() {
  return pattern_payloads_;
}
template <>
inline constexpr base::Arena<TypePayload>& HirContext::arena<TypePayload>() {
  return type_payloads_;
}
template <>
inline constexpr base::Arena<DeclarationPayload>&
HirContext::arena<DeclarationPayload>() {
  return item_payloads_;
}
template <>
inline constexpr base::Arena<ResolvedPathPayload>&
HirContext::arena<ResolvedPathPayload>() {
  return resolved_path_payloads_;
}
template <>
inline constexpr base::Arena<GenericArgPayload>&
HirContext::arena<GenericArgPayload>() {
  return generic_arg_payloads_;
}
template <>
inline constexpr base::Arena<ParameterPayload>&
HirContext::arena<ParameterPayload>() {
  return parameter_payloads_;
}
template <>
inline constexpr base::Arena<FieldPayload>& HirContext::arena<FieldPayload>() {
  return field_payloads_;
}
template <>
inline constexpr base::Arena<EnumVariantPayload>&
HirContext::arena<EnumVariantPayload>() {
  return enum_variant_payloads_;
}
template <>
inline constexpr base::Arena<MatchArmPayload>&
HirContext::arena<MatchArmPayload>() {
  return match_arm_payloads_;
}
template <>
inline constexpr base::Arena<CapturePayload>&
HirContext::arena<CapturePayload>() {
  return capture_payloads_;
}
template <>
inline constexpr base::Arena<StructPatternFieldPayload>&
HirContext::arena<StructPatternFieldPayload>() {
  return struct_pattern_field_payloads_;
}
template <>
inline constexpr base::Arena<VisibilityPayload>&
HirContext::arena<VisibilityPayload>() {
  return visibility_payloads_;
}

template <>
inline constexpr base::Arena<HirId>& HirContext::arena<HirId>() {
  return hir_ids_;
}
template <>
inline constexpr base::Arena<DefId>& HirContext::arena<DefId>() {
  return def_ids_;
}
template <>
inline constexpr base::Arena<LocalId>& HirContext::arena<LocalId>() {
  return local_ids_;
}

}  // namespace hir

#endif  // FRONTEND_DATA_HIR_CONTEXT_H_
