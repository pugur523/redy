// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "frontend/diagnostic/data/severity.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/data/translation_key.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_struct_declaration(
    PayloadId attribute) {
  auto struct_r = consume(base::TokenKind::kStruct, true);
  if (struct_r.is_err()) {
    return err<NodeId>(std::move(struct_r).unwrap_err());
  }

  auto struct_name_r = consume(base::TokenKind::kIdentifier, true);
  if (struct_name_r.is_err()) {
    return err<NodeId>(std::move(struct_name_r).unwrap_err());
  }
  const std::string_view struct_name = peek().lexeme(stream_->file());

  auto left_r = consume(base::TokenKind::kLeftBrace, true);
  if (left_r.is_err()) {
    return err<NodeId>(std::move(left_r).unwrap_err());
  }

  auto fields_r = parse_field_list();
  if (fields_r.is_err()) {
    return err<NodeId>(std::move(fields_r).unwrap_err());
  }
  const ast::PayloadRange fields_range = std::move(fields_r).unwrap();

  auto right_r = consume(base::TokenKind::kRightBrace, true);
  if (right_r.is_err()) {
    return err<NodeId>(std::move(right_r).unwrap_err());
  }

  const PayloadId payload_id = context_->alloc(ast::StructDeclarationPayload{
      .name = struct_name,
      .fields_range = fields_range,
      .storage_attribute = attribute,
  });

  return ok(context_->alloc(ast::Node{
      .kind = ast::NodeKind::kFunctionDeclaration,
      .payload_id = payload_id,
  }));
}

}  // namespace parser
