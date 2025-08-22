// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::PayloadId> Parser::parse_capture_one() {
  auto capture_name_r = consume(base::TokenKind::kIdentifier, true);
  if (capture_name_r.is_err()) {
    return err<PayloadId>(std::move(capture_name_r).unwrap_err());
  }
  const std::string_view capture_name =
      std::move(capture_name_r).unwrap()->lexeme(stream_->file());

  auto colon_r = consume(base::TokenKind::kColon, true);
  if (colon_r.is_err()) {
    return err<PayloadId>(std::move(colon_r).unwrap_err());
  }

  auto type_r = parse_type_reference();
  if (type_r.is_err()) {
    return err<PayloadId>(std::move(type_r).unwrap_err());
  }
  const NodeId type = std::move(type_r).unwrap();

  return ok(context_->alloc(ast::CapturePayload{
      .name = capture_name,
      .type = type,
  }));
}

Parser::Result<ast::PayloadRange> Parser::parse_capture_list() {
  uint32_t captures_count = 0;
  ast::PayloadId id = ast::kInvalidPayloadId;
  while (!eof() && peek().kind() != base::TokenKind::kRightBracket) {
    auto r = parse_capture_one();
    if (r.is_err()) {
      return err<ast::PayloadRange>(std::move(r).unwrap_err());
    } else if (captures_count == 0) {
      id = std::move(r).unwrap();
    }
    ++captures_count;
    next_non_whitespace();
  }

  // returns ok even if id is invalid and captures count is 0
  return ok(ast::PayloadRange{
      .begin = id,
      .size = captures_count,
  });
}

}  // namespace parser
