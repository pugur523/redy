// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/parser/parser.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "core/base/logger.h"
#include "core/location.h"
#include "frontend/ast/base/base_node.h"
#include "frontend/ast/nodes/node_util.h"
#include "frontend/base/token/token_kind.h"
#include "frontend/base/token/token_stream.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/parser/parse_error.h"

namespace parser {

Parser::Parser(base::TokenStream&& stream, bool strict)
    : stream_(std::move(stream)), strict_(strict) {}

// program ::= { statement } ;
Parser::Results<Parser::AstNode> Parser::parse() {
  std::vector<AstNode> statements;
  std::vector<ParseError> errors;

  const auto& start_token = peek();
  while (!eof()) {
    Results<AstNode> result =
        check(base::TokenKind::kFn) ? parse_function() : parse_statement();

    if (result.is_err()) {
      append_errs(&errors, std::move(result).unwrap_err());
      if (strict_) {
        return err(std::move(errors));
      }
      synchronize();
    } else {
      statements.push_back(std::move(result).unwrap());
    }
  }

  if (errors.empty()) {
    return ok(
        ast::make_node<ast::ProgramNode>(start_token, std::move(statements)));
  } else {
    return err(std::move(errors));
  }
}

Parser::Result<const base::Token*> Parser::consume(
    base::TokenKind expected,
    std::string&& error_message) {
  const auto& token = peek();
  if (check(expected)) {
    advance();
    return Result<const base::Token*>(diagnostic::make_ok(&token));
  } else {
    return Result<const base::Token*>(diagnostic::make_err(
        ParseError::make(diagnostic::DiagnosticId::kUnexpectedToken, token,
                         std::move(error_message))));
  }
}

void Parser::synchronize() {
  if (eof()) {
    return;
  }
  advance();
  while (!eof()) {
    if (previous().kind() == base::TokenKind::kSemicolon) {
      return;
    }
    if (is_sync_point(peek().kind())) {
      return;
    }
    advance();
  }
}

// static
bool Parser::is_sync_point(base::TokenKind kind) {
  using Tk = base::TokenKind;
  switch (kind) {
    case Tk::kFunction:
    case Tk::kIf:
    case Tk::kWhile:
    case Tk::kFor:
    case Tk::kReturn:
    case Tk::kMutable:
    case Tk::kStruct:
    case Tk::kEnumeration: return true;
    default: return false;
  }
}

// static
Parser::Result<Parser::AstNode> Parser::single_ok(AstNode&& node) {
  return Result<AstNode>(diagnostic::make_ok(std::move(node)));
}

// static
Parser::Result<Parser::AstNode> Parser::single_err(ParseError&& error) {
  return Result<AstNode>(diagnostic::make_err(std::move(error)));
}

// static
Parser::Results<Parser::AstNode> Parser::ok(AstNode&& node) {
  return Results<AstNode>(diagnostic::make_ok(std::move(node)));
}

// static
Parser::Results<Parser::AstNode> Parser::err(std::vector<ParseError>&& errors) {
  return Results<AstNode>(diagnostic::make_err(std::move(errors)));
}

// static
void Parser::append_errs(std::vector<ParseError>* target,
                         std::vector<ParseError>&& source) {
  DCHECK(target);
  target->insert(target->end(), std::make_move_iterator(source.begin()),
                 std::make_move_iterator(source.end()));
}

}  // namespace parser
