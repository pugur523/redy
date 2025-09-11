// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/processor/parser/parser.h"

#include <iterator>
#include <utility>

#include "frontend/base/keyword/attribute_keyword.h"
#include "frontend/base/keyword/control_flow_keyword.h"
#include "frontend/base/keyword/declaration_keyword.h"
#include "frontend/base/keyword/keyword.h"
#include "frontend/base/keyword/modifier_keyword.h"
#include "frontend/base/operator/binary_operator.h"
#include "frontend/base/operator/unary_operator.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/data/ast/base/node_id.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/diagnostic/data/severity.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"
#include "unicode/utf8/file_manager.h"

namespace parser {

void Parser::init(base::TokenStream* stream,
                  base::StringInterner* interner,
                  const i18n::Translator& translator) {
  DCHECK_EQ(status_, Status::kNotInitialized);
  stream_ = stream;
  DCHECK(stream_);

  interner_ = interner;
  DCHECK(interner_);

  translator_ = &translator;
  DCHECK(translator_);

  init_context();

  status_ = Status::kReadyToParse;
  return;
}

void Parser::init_context() {
  DCHECK_EQ(status_, Status::kNotInitialized);
  context_ = ast::Context::create();
  DCHECK(context_);

  // TODO: hot path heuristic allocations
  context_->arena<ast::Node>().reserve(512);

  // expressions
  context_->arena<ast::LiteralExpressionPayload>().reserve(256);
  context_->arena<ast::PathExpressionPayload>().reserve(256);
  context_->arena<ast::UnaryExpressionPayload>().reserve(256);
  context_->arena<ast::BinaryExpressionPayload>().reserve(256);
  context_->arena<ast::GroupedExpressionPayload>().reserve(32);
  context_->arena<ast::ArrayExpressionPayload>().reserve(32);
  context_->arena<ast::TupleExpressionPayload>().reserve(32);
  context_->arena<ast::IndexExpressionPayload>().reserve(32);
  context_->arena<ast::ConstructExpressionPayload>().reserve(256);
  context_->arena<ast::FunctionCallExpressionPayload>().reserve(512);
  context_->arena<ast::MethodCallExpressionPayload>().reserve(256);
  context_->arena<ast::FunctionMacroCallExpressionPayload>().reserve(256);
  context_->arena<ast::MethodMacroCallExpressionPayload>().reserve(256);
  context_->arena<ast::FieldAccessExpressionPayload>().reserve(256);
  context_->arena<ast::AwaitExpressionPayload>().reserve(256);
  context_->arena<ast::ContinueExpressionPayload>().reserve(256);
  context_->arena<ast::BreakExpressionPayload>().reserve(256);
  context_->arena<ast::RangeExpressionPayload>().reserve(256);
  context_->arena<ast::ReturnExpressionPayload>().reserve(512);
  context_->arena<ast::BlockExpressionPayload>().reserve(256);
  context_->arena<ast::BlockExpressionPayload>().reserve(256);
  context_->arena<ast::IfExpressionPayload>().reserve(256);
  context_->arena<ast::LoopExpressionPayload>().reserve(256);
  context_->arena<ast::WhileExpressionPayload>().reserve(256);
  context_->arena<ast::ForExpressionPayload>().reserve(256);
  context_->arena<ast::MatchExpressionPayload>().reserve(256);
  context_->arena<ast::ClosureExpressionPayload>().reserve(256);

  // statements
  context_->arena<ast::AssignStatementPayload>().reserve(512);
  context_->arena<ast::AttributeStatementPayload>().reserve(32);
  context_->arena<ast::ExpressionStatementPayload>().reserve(512);
  context_->arena<ast::UseStatementPayload>().reserve(32);

  context_->arena<ast::FunctionDeclarationPayload>().reserve(512);
  context_->arena<ast::StructDeclarationPayload>().reserve(256);
  context_->arena<ast::EnumerationDeclarationPayload>().reserve(256);
  context_->arena<ast::TraitDeclarationPayload>().reserve(256);
  context_->arena<ast::ImplementationDeclarationPayload>().reserve(256);
  context_->arena<ast::UnionDeclarationPayload>().reserve(256);
  context_->arena<ast::ModuleDeclarationPayload>().reserve(32);
  context_->arena<ast::RedirectDeclarationPayload>().reserve(256);

  // data
  context_->arena<ast::AttributeUsePayload>().reserve(256);
  context_->arena<ast::CapturePayload>().reserve(256);
  context_->arena<ast::FieldPayload>().reserve(256);
  context_->arena<ast::ParameterPayload>().reserve(256);
  context_->arena<ast::EnumVariantPayload>().reserve(256);
  context_->arena<ast::TypeReferencePayload>().reserve(256);
  context_->arena<ast::ArrayTypePayload>().reserve(256);
  context_->arena<ast::IdentifierPayload>().reserve(256);
  context_->arena<ast::IfBranchPayload>().reserve(256);
  context_->arena<ast::MatchArmPayload>().reserve(256);
}

Parser::ParseResult Parser::parse_all(bool strict) {
  DCHECK_EQ(status_, Status::kReadyToParse);
  while (!eof()) {
    auto result = parse_next();
    if (result.is_err()) [[unlikely]] {
      errors_.emplace_back(std::move(result).unwrap_err());
      if (strict) {
        break;
      } else {
        synchronize();
        continue;
      }
    }
  }

  if (errors_.empty()) [[likely]] {
    status_ = Status::kParseCompleted;
    return ParseResult(diagnostic::create_ok(std::move(context_)));
  } else {
    status_ = Status::kErrorOccured;
    return ParseResult(diagnostic::create_err(std::move(errors_)));
  }
}

Parser::Result<void> Parser::parse_next() {
  const base::TokenKind current_kind = peek().kind();

  // parses declaration or statement
  if (eof() || current_kind == base::TokenKind::kEof) {
    return ok<void>();
  } else if (current_kind == base::TokenKind::kBlockComment ||
             current_kind == base::TokenKind::kInlineComment) {
    // TODO: support document gen mode
    next();
    return ok<void>();
  } else if (current_kind == base::TokenKind::kSemicolon) {
    // consume ;
    next();
    return ok<void>();
  } else {
    auto result = parse_statement();
    if (result.is_err()) {
      return err<void>(std::move(result));
    }
    return ok<void>();
  }
}

void Parser::append_errors(std::vector<De>&& new_errors) {
  errors_.insert(errors_.end(), std::make_move_iterator(new_errors.begin()),
                 std::make_move_iterator(new_errors.end()));
}

Parser::Result<const base::Token*> Parser::consume(base::TokenKind expected,
                                                   bool skip_whitespaces) {
  const base::Token& token = stream_->peek();
  if (check(expected)) [[likely]] {
    if (skip_whitespaces) {
      next_non_whitespace();
    } else {
      next();
    }
    return ok<const base::Token*>(&token);
  } else {
    return err<const base::Token*>(
        std::move(
            Eb(diagnostic::Severity::kError,
               diagnostic::DiagId::kExpectedButFound)
                .label(stream_->file_id(), token.range(),
                       i18n::TranslationKey::kDiagnosticParserExpectedButFound,
                       diagnostic::LabelMarkerType::kEmphasis,
                       {translator_->translate(
                            base::token_kind_to_tr_key(expected)),
                        translator_->translate(
                            base::token_kind_to_tr_key(token.kind()))}))
            .build());
  }
}

void Parser::synchronize() {
  if (eof()) [[unlikely]] {
    return;
  }

  // consume errored token
  next();
  base::TokenKind kind = peek().kind();
  while (!eof()) {
    if (is_sync_point(kind)) {
      return;
    }

    switch (kind) {
      // delimiters
      case base::TokenKind::kSemicolon:
      case base::TokenKind::kRightBrace:
      case base::TokenKind::kNewline: next(); return;
      case base::TokenKind::kEof: return;
      default: break;
    }
    kind = next().kind();
  }
}

// static
bool Parser::is_sync_point(base::TokenKind kind) {
  return base::token_kind_is_control_flow_keyword(kind) ||
         base::token_kind_is_declaration_keyword(kind) ||
         base::token_kind_is_attribute_keyword(kind);
}

}  // namespace parser
