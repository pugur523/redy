// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/operator/operator.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/payload/common.h"
#include "frontend/data/ast/payload/expression.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

using R = ast::PayloadId<ast::RangeExpressionPayload>;

Parser::Result<R> Parser::parse_range_expr() {
  // avoid inifinite loop
  auto begin_r = parse_binary_expr(base::OperatorPrecedence::kLowest);
  if (begin_r.is_err()) {
    return err<R>(std::move(begin_r));
  }

  if (!check(base::TokenKind::kDotDot)) {
    return ok(context_->alloc_payload(ast::RangeExpressionPayload{
        .begin = std::move(begin_r).unwrap(),
        .end = ast::kInvalidNodeId,
        .is_exclusive = true,
    }));
  }

  // consume ..
  next_non_whitespace();

  bool is_exclusive = true;

  const base::Token& lt_or_equal = peek();
  switch (lt_or_equal.kind()) {
    case base::TokenKind::kLt: /* is_exclusive = true; */ break;
    case base::TokenKind::kEqual: is_exclusive = false; break;
    default:
      return err<R>((
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagId::kUnexpectedToken)
                  .label(stream_->file_id(), lt_or_equal.range(),
                         i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                         diagnostic::LabelMarkerType::kEmphasis))
              .build()));
  }

  auto end_r = parse_binary_expr(base::OperatorPrecedence::kLowest);
  if (end_r.is_err()) {
    return err<R>(std::move(end_r));
  }

  return ok(context_->alloc_payload(ast::RangeExpressionPayload{
      .begin = std::move(begin_r).unwrap(),
      .end = std::move(end_r).unwrap(),
      .is_exclusive = is_exclusive,
  }));
}

}  // namespace parser
