// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_primary_expression() {
  const base::TokenKind kind = peek().kind();

  if (base::token_kind_is_literal(kind)) {
    return parse_literal_expression();
  }
  switch (kind) {
    case base::TokenKind::kIdentifier: return parse_postfix_expression();
    case base::TokenKind::kBreak: return parse_break_expression();
    case base::TokenKind::kContinue: return parse_continue_expression();
    case base::TokenKind::kReturn: return parse_return_expression();
    case base::TokenKind::kLeftParen: return parse_grouped_expression();

    case base::TokenKind::kLeftBrace: return parse_block_expression();
    case base::TokenKind::kUnsafe: return parse_unsafe_expression();
    case base::TokenKind::kFast: return parse_fast_expression();
    case base::TokenKind::kIf: return parse_if_expression();
    case base::TokenKind::kLoop: return parse_loop_expression();
    case base::TokenKind::kWhile: return parse_while_expression();
    case base::TokenKind::kFor: return parse_for_expression();
    case base::TokenKind::kMatch: return parse_match_expression();
    case base::TokenKind::kLeftBracket: return parse_closure_expression();
    default:
      return err<NodeId>(
          std::move(
              Eb(diagnostic::Severity::kError,
                 diagnostic::DiagnosticId::kUnexpectedToken)
                  .label(stream_->file_id(), peek().range(),
                         i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                         diagnostic::LabelMarkerType::kLine))
              .build());
  }
}

}  // namespace parser
