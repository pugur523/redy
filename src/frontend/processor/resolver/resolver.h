// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_
#define FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_

#include "frontend/processor/resolver/base/resolver_export.h"

namespace resolver {

class RESOLVER_EXPORT Resolver {
 public:
  Resolver() = default;
  ~Resolver() = default;

  Resolver(const Resolver&) = delete;
  Resolver& operator=(const Resolver&) = delete;

  Resolver(Resolver&&) noexcept = default;
  Resolver& operator=(Resolver&&) noexcept = default;
};

}  // namespace resolver

#endif  // FRONTEND_PROCESSOR_RESOLVER_RESOLVER_H_
