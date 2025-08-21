// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/node_kind.h"
#include "frontend/data/ast/base/payload.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_range_expression() {
  auto begin_r = parse_binary_expression(base::OperatorPrecedence::kLowest);
  if (begin_r.is_err()) {
    return begin_r;
  }

  auto dot_dot_r = consume(base::TokenKind::kDotDot, true);
  if (dot_dot_r.is_err()) {
    return err<NodeId>(std::move(dot_dot_r).unwrap_err());
  }

  bool is_exclusive;

  const base::Token& lt_or_equal = peek();
  switch (lt_or_equal.kind()) {
    case base::TokenKind::kLt: is_exclusive = true; break;
    case base::TokenKind::kEqual: is_exclusive = false; break;
    default:
      return err<NodeId>(
          (std::move(
               Eb(diagnostic::Severity::kError,
                  diagnostic::DiagId::kUnexpectedToken)
                   .label(
                       stream_->file_id(), lt_or_equal.range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis))
               .build()));
  }

  auto end_r = parse_binary_expression(base::OperatorPrecedence::kLowest);
  if (end_r.is_err()) {
    return end_r;
  }

  ast::NodeKind kind;
  PayloadId payload_id;
  if (is_exclusive) {
    kind = ast::NodeKind::kExclusiveRangeExpression;
    payload_id = context_->alloc(ast::ExclusiveRangeExpressionPayload{
        .begin = std::move(begin_r).unwrap(),
        .end = std::move(end_r).unwrap(),
    });
  } else {
    kind = ast::NodeKind::kInclusiveRangeExpression;
    payload_id = context_->alloc(ast::InclusiveRangeExpressionPayload{
        .begin = std::move(begin_r).unwrap(),
        .end = std::move(end_r).unwrap(),
    });
  }

  return ok(context_->alloc(ast::Node{
      .kind = kind,
      .payload_id = payload_id,
  }));
}

}  // namespace parser
