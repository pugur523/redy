// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/parser/parser.h"

#include "frontend/diagnostic/data/result.h"

namespace parser {

void Parser::init(const base::TokenStream& stream) {
  stream_ = &stream;
  context_ = ast::Context::create();
  status_ = Status::kReadyToParse;
  return;
}

}  // namespace parser
