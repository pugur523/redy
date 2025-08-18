// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include <utility>

#include "frontend/base/literal/literal.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/data/ast/base/nodes.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/processor/parser/parser.h"
#include "i18n/base/translator.h"

namespace parser {

Parser::Result<ast::NodeId> Parser::parse_primary_expression() {
  const base::TokenKind kind = peek().kind();

  if (base::token_kind_is_literal(kind)) {
    return parse_literal_expression();
  } else {
    switch (kind) {
      case base::TokenKind::kIdentifier: {
        switch (peek_at(1).kind()) {
          case base::TokenKind::kColonColon: return parse_path_expression();
          case base::TokenKind::kLeftParen:
            return parse_function_call_expression();
          case base::TokenKind::kHash:
            return parse_function_macro_call_expression();
          case base::TokenKind::kDot: {
            // method call / macro method call / field access
            const base::Token& second = peek_at(2);
            if (second.kind() != base::TokenKind::kIdentifier) {
              return err<ast::NodeId>(
                  std::move(Eb(diagnostic::Severity::kError,
                               diagnostic::DiagnosticId::kUnexpectedToken)
                                .label(stream_->file_id(), second.range(),
                                       i18n::TranslationKey::
                                           kDiagnosticParserUnexpectedToken,
                                       diagnostic::LabelMarkerType::kLine,
                                       {translator_->translate(
                                           base::token_kind_to_tr_key(
                                               second.kind()))}))
                      .build());
            }

            // some_obj.some_identifier
            const base::Token& third = peek_at(3);
            if (third.kind() == base::TokenKind::kLeftParen) {
              // some_obj.some_method(
              return parse_method_call_expression();
            } else if (third.kind() == base::TokenKind::kHash) {
              // some_obj.some_macro#
              return parse_method_macro_call_expression();
            } else {
              // some_obj.some_field
              return parse_field_access_expression();
            }
          }
          default:
            // raw identifier
            return ok(context_->alloc(ast::IdentifierNode{
                .lexeme = peek().lexeme(stream_->file()),
            }));
        }
      }
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
        return err<ast::NodeId>(
            std::move(
                Eb(diagnostic::Severity::kError,
                   diagnostic::DiagnosticId::kUnexpectedToken)
                    .label(
                        stream_->file_id(), peek().range(),
                        i18n::TranslationKey::kDiagnosticParserUnexpectedToken,
                        diagnostic::LabelMarkerType::kLine))
                .build());
    }
  }
}

}  // namespace parser
