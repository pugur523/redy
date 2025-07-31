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
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/result.h"
#include "frontend/lexer/token/token_kind.h"
#include "frontend/lexer/token/token_stream.h"
#include "frontend/parser/parse_error.h"

namespace parser {

Parser::Parser(lexer::TokenStream&& stream, bool strict)
    : stream_(std::move(stream)), strict_(strict) {}

// program ::= { function | statement } ;
Parser::Results<Parser::AstNode> Parser::parse() {
  std::vector<AstNode> statements;
  std::vector<ParseError> errors;

  const auto& start_token = peek();
  while (!eof()) {
    Results<AstNode> result =
        check(lexer::TokenKind::kFn) ? parse_function() : parse_statement();

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

Parser::Result<const lexer::Token*> Parser::consume(
    lexer::TokenKind expected,
    std::string&& error_message) {
  const auto& token = peek();
  if (check(expected)) {
    advance();
    return Result<const lexer::Token*>(diagnostic::make_ok(&token));
  } else {
    return Result<const lexer::Token*>(diagnostic::make_err(
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
    if (previous().kind() == lexer::TokenKind::kSemicolon) {
      return;
    }
    if (is_sync_point(peek().kind())) {
      return;
    }
    advance();
  }
}

// static
bool Parser::is_sync_point(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;
  switch (kind) {
    case Tk::kFn:
    case Tk::kIf:
    case Tk::kWhile:
    case Tk::kFor:
    case Tk::kReturn:
    case Tk::kMut:
    case Tk::kStruct:
    case Tk::kEnum: return true;
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

// static
ast::BinaryOpNode::Operator Parser::token_to_binary_op(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;
  using Op = ast::BinaryOpNode::Operator;
  switch (kind) {
    // arithmetic operators
    case Tk::kPlus: return Op::kAdd;
    case Tk::kMinus: return Op::kSub;
    case Tk::kStar: return Op::kMul;
    case Tk::kSlash: return Op::kDiv;
    case Tk::kPercent: return Op::kMod;
    case Tk::kStarStar: return Op::kPow;

    // comparison operators
    case Tk::kEqEq: return Op::kEqual;
    case Tk::kNeq: return Op::kNotEqual;
    case Tk::kLt: return Op::kLess;
    case Tk::kLe: return Op::kLessEqual;
    case Tk::kGt: return Op::kGreater;
    case Tk::kGe: return Op::kGreaterEqual;

    // logical operators
    case Tk::kAndAnd: return Op::kAnd;
    case Tk::kPipePipe: return Op::kOr;

    // bitwise operators
    case Tk::kAmp: return Op::kBitwiseAnd;    // &
    case Tk::kPipe: return Op::kBitwiseOr;    // |
    case Tk::kCaret: return Op::kBitwiseXor;  // ^
    case Tk::kLtLt: return Op::kLeftShift;    // <<
    case Tk::kGtGt:
      return Op::kRightShift;  // >>

    // assignment operators
    case Tk::kPlusEq: return Op::kAddAssign;          // +=
    case Tk::kMinusEq: return Op::kSubAssign;         // -=
    case Tk::kStarEq: return Op::kMulAssign;          // *=
    case Tk::kSlashEq: return Op::kDivAssign;         // /=
    case Tk::kPercentEq: return Op::kModAssign;       // %=
    case Tk::kAmpEq: return Op::kBitwiseAndAssign;    // &=
    case Tk::kPipeEq: return Op::kBitwiseOrAssign;    // |=
    case Tk::kCaretEq: return Op::kBitwiseXorAssign;  // ^=
    case Tk::kLtLtEq: return Op::kLeftShiftAssign;    // <<=
    case Tk::kGtGtEq: return Op::kRightShiftAssign;   // >>=

    default: return Op::kUnknown;
  }
}

// static
ast::UnaryOpNode::Operator Parser::token_to_unary_op(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;
  using Op = ast::UnaryOpNode::Operator;

  switch (kind) {
    case Tk::kBang: return Op::kNot;          // ! (logical NOT)
    case Tk::kMinus: return Op::kNegate;      // - (unary minus)
    case Tk::kTilde: return Op::kBitwiseNot;  // ~ (bitwise NOT)
    case Tk::kPlusPlus: return Op::kIncrement;
    case Tk::kMinusMinus: return Op::kDecrement;

    default: return Op::kUnknown;
  }
}

// determines operator precedence for binary operators only.
// higher return value means higher precedence.
// static
int Parser::get_precedence(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;

  switch (kind) {
    case Tk::kPipePipe: return 1;  // || (lowest)
    case Tk::kAndAnd: return 2;    // &&
    case Tk::kEqEq:
    case Tk::kNeq: return 3;  // ==, !=
    case Tk::kLt:
    case Tk::kLe:
    case Tk::kGt:
    case Tk::kGe: return 4;     // <, <=, >, >=
    case Tk::kAmp: return 5;    // & (bitwise AND)
    case Tk::kCaret: return 6;  // ^ (bitwise XOR)
    case Tk::kPipe: return 7;   // | (bitwise OR)
    case Tk::kLtLt:
    case Tk::kGtGt: return 8;  // <<, >>
    case Tk::kPlus:
    case Tk::kMinus: return 9;  // +, -
    case Tk::kStar:
    case Tk::kSlash:
    case Tk::kPercent: return 10;   // *, /, %
    case Tk::kStarStar: return 11;  // ** (highest for binary ops)
    default: return 0;  // Not a binary operator with a specific precedence
  }
}

// check if a token kind represents a binary operator (excluding assignment
// operators)
// static
bool Parser::is_binary_operator(lexer::TokenKind kind) {
  return get_precedence(kind) > 0;
}

// check if a token kind represents a unary operator
// static
bool Parser::is_unary_operator(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;

  switch (kind) {
    case Tk::kBang:        // !
    case Tk::kMinus:       // unary -
    case Tk::kTilde:       // ~
    case Tk::kPlusPlus:    // ++
    case Tk::kMinusMinus:  // --
      return true;
    default: return false;
  }
}

// check if a token kind represents an assignment operator
// static
bool Parser::is_assignment_operator(lexer::TokenKind kind) {
  using Tk = lexer::TokenKind;

  switch (kind) {
    case Tk::kAssign:
    case Tk::kPlusEq:
    case Tk::kMinusEq:
    case Tk::kStarEq:
    case Tk::kSlashEq:
    case Tk::kPercentEq:
    case Tk::kAmpEq:
    case Tk::kPipeEq:
    case Tk::kCaretEq:
    case Tk::kLtLtEq:
    case Tk::kGtGtEq: return true;
    default: return false;
  }
}

}  // namespace parser
