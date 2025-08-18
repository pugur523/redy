// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_expression() {
  return parse_binary_expression(base::OperatorPrecedence::kLowest);
}

}  // namespace parser
