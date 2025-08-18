// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_array_expression() {
  auto left_r = consume(base::TokenKind::kLeftBracket, true);
  if (left_r.is_err()) {
    return err<ast::NodeId>(std::move(left_r).unwrap_err());
  }

  ast::NodeId first_id = ast::kInvalidNodeId;
  uint32_t elements_count = 0;

  while (!eof()) {
    if (peek().kind() == base::TokenKind::kRightBracket) {
      break;
    }

    auto expr_r = parse_expression();
    if (expr_r.is_err()) {
      return expr_r;
    }
    if (elements_count == 0) {
      first_id = std::move(expr_r).unwrap();
    }

    ++elements_count;

    const base::Token& next_token = next_non_whitespace();
    if (next_token.kind() == base::TokenKind::kComma) {
      next_non_whitespace();
    } else {
      return err<ast::NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagId::kUnexpectedToken)
                  .label(stream_->file_id(), next_token.range(),
                         i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                         diagnostic::LabelMarkerType::kEmphasis))
              .build());
    }
  }

  auto right_r = consume(base::TokenKind::kRightBracket, true);
  if (right_r.is_err()) {
    return err<ast::NodeId>(std::move(right_r).unwrap_err());
  }

  return ok(context_->alloc(ast::ArrayExpressionNode{
      .array_elements_range = {.begin = first_id, .size = elements_count},
  }));
}

}  // namespace parser

