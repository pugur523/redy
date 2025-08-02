// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_
#define FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_

#include <cstdint>

#include "frontend/diagnostic/base/style.h"

namespace diagnostic {

// enum class DiagnosticType : uint8_t {
//   kGeneric = 0,
//   kLex = 1,
//   kParse = 2,
//   kSema = 3,
//   kHir = 4,
//   kMir = 5,
//   kCodegen = 6,
// };

enum class Severity : uint8_t {
  kUnknown = 0,
  kFatal = 1,
  kError = 2,
  kWarn = 3,
  kInfo = 4,
  kDebug = 5,
  kTrace = 6,
};

inline const char* severity_to_string(Severity severity) {
  switch (severity) {
    case Severity::kUnknown: return "unknown";
    case Severity::kTrace: return "trace";
    case Severity::kDebug: return "debug";
    case Severity::kInfo: return "info";
    case Severity::kWarn: return "warn";
    case Severity::kError: return "error";
    case Severity::kFatal: return "fatal";
  }
}

inline Style severity_to_style(Severity severity) {
  switch (severity) {
    case Severity::kUnknown: return Style::kUnknown;
    case Severity::kFatal: return Style::kMagenta;
    case Severity::kError: return Style::kRed;
    case Severity::kWarn: return Style::kYellow;
    case Severity::kInfo: return Style::kGreen;
    case Severity::kDebug: return Style::kCyan;
    case Severity::kTrace: return Style::kGray;
  }
}

enum class DiagnosticId : uint8_t {
  kUnknown = 0,

  // lexer
  kInvalidToken = 1,
  kUnterminatedStringLiteral = 2,
  kUnterminatedCharacterLiteral = 3,
  kUnterminatedBlockComment = 4,
  kUnrecognizedCharacter = 5,
  kInvalidEscapeSequence = 6,
  kInvalidNumericLiteral = 7,
  kNumericLiteralOutOfRange = 8,
  kUnexpectedEndOfFile = 9,

  // parser
  kUnexpectedToken = 10,
  kMissingToken = 11,
  kExpectedSemicolon = 12,
  kExpectedIdentifier = 13,
  kExpectedType = 14,
  kExpectedLParen = 15,
  kExpectedRParen = 16,
  kExpectedLBrace = 17,
  kExpectedRBrace = 18,
  kExpectedLBracket = 19,
  kExpectedRBracket = 20,
  kExpectedExpression = 21,
  kExpectedReturnExpression = 22,
  kExpectedBlock = 23,
  kUnexpectedKeyword = 24,
  kMalformedDeclaration = 25,
  kDuplicateParameterName = 26,
  kParameterCountMismatch = 27,
  kInvalidFunctionCall = 28,
  kInvalidAssignmentTarget = 29,
  kInvalidGenericArguments = 30,
  kBreakOutsideLoop = 31,
  kContinueOutsideLoop = 32,
  kInvalidPattern = 33,
  kInvalidSyntax = 34,  // fallback

  // sema (related to declaration & type)
  kUndefinedSymbol = 35,
  kUndefinedVariable = 36,
  kUndefinedFunction = 37,
  kUndefinedType = 38,
  kCallArgumentMismatch = 39,
  kReturnTypeMismatch = 40,
  kNonCallableExpression = 41,
  kInvalidOperatorOperands = 42,
  kMemberNotFound = 43,
  kAccessPrivateMember = 44,
  kImmutableBindingChanged = 45,
  kConstAssignment = 46,
  kTypeMismatch = 47,
  kTypeAnnotationRequired = 48,
  kNonIterableExpression = 49,
  kInfiniteLoopLiteral = 50,
  kFunctionSignatureMismatch = 51,
  kRedeclaration = 52,
  kConflictingDeclaration = 53,
  kConflictingTraitImplementation = 54,
  kMissingTraitBound = 55,
  kVariableNotInitialized = 56,
  kMisplacedAttribute = 57,
  kRecursiveTypeDefinition = 58,
  kCyclicDependency = 59,

  // lifetime / borrow checker
  kDanglingReference = 60,
  kUnusedLifetimeParameter = 61,
  kUnusedBorrow = 62,
  kLifetimeConflict = 63,
  kLifetimeAnnotationRequired = 64,
  kReturnedBorrowDoesNotLiveLongEnough = 65,
  kMoveAfterBorrow = 66,
  kBorrowAfterMove = 67,
  kUseAfterMove = 68,
  kMultipleMutBorrow = 69,
  kMutableAlias = 70,
  kImmutableBorrowIntoMutable = 71,

  // warnings
  kUnusedVariable = 72,
  kUnusedFunction = 73,
  kUnreachableCode = 74,
  kImplicitConversion = 75,
  kMissingReturnStatement = 76,
  kDeprecatedFeature = 77,
  kDeprecatedApiUsage = 78,
  kAmbiguousCall = 79,
  kUnnecessaryCopy = 80,
  kShadowingVariable = 81,
  kNumericDivisionByZero = 82,
  kAlwaysTrueCondition = 83,
  kAlwaysFalseCondition = 84,
  kMissingDefaultCase = 85,
  kInefficientLoop = 86,
  kRedundantCast = 87,
  kEmptyLoopBody = 88,
  kIneffectiveAssignment = 89,
};

inline const char* diagnostic_id_to_str(DiagnosticId id) {
  using Id = DiagnosticId;
  switch (id) {
    case Id::kUnknown: return "unknown";

    // lexer
    case Id::kInvalidToken: return "invalid_token";
    case Id::kUnterminatedStringLiteral: return "unterminated_string_literal";
    case Id::kUnterminatedCharacterLiteral:
      return "unterminated_character_literal";
    case Id::kUnterminatedBlockComment: return "unterminated_block_comment";
    case Id::kUnrecognizedCharacter: return "unrecognized_character";
    case Id::kInvalidEscapeSequence: return "invalid_escape_sequence";
    case Id::kInvalidNumericLiteral: return "invalid_numeric_literal";
    case Id::kNumericLiteralOutOfRange: return "numeric_literal_out_of_range";
    case Id::kUnexpectedEndOfFile: return "unexpected_end_of_file";

    // parser
    case Id::kUnexpectedToken: return "unexpected_token";
    case Id::kMissingToken: return "missing_token";
    case Id::kExpectedSemicolon: return "expected_semicolon";
    case Id::kExpectedIdentifier: return "expected_identifier";
    case Id::kExpectedType: return "expected_type";
    case Id::kExpectedLParen: return "expected_lparen";
    case Id::kExpectedRParen: return "expected_rparen";
    case Id::kExpectedLBrace: return "expected_lbrace";
    case Id::kExpectedRBrace: return "expected_rbrace";
    case Id::kExpectedLBracket: return "expected_lbracket";
    case Id::kExpectedRBracket: return "expected_rbracket";
    case Id::kExpectedExpression: return "expected_expression";
    case Id::kExpectedReturnExpression: return "expected_return_expression";
    case Id::kExpectedBlock: return "expected_block";
    case Id::kUnexpectedKeyword: return "unexpected_keyword";
    case Id::kMalformedDeclaration: return "malformed_declaration";
    case Id::kDuplicateParameterName: return "duplicate_parameter_name";
    case Id::kParameterCountMismatch: return "parameter_count_mismatch";
    case Id::kInvalidFunctionCall: return "invalid_function_call";
    case Id::kInvalidAssignmentTarget: return "invalid_assignment_target";
    case Id::kInvalidGenericArguments: return "invalid_generic_arguments";
    case Id::kBreakOutsideLoop: return "break_outside_loop";
    case Id::kContinueOutsideLoop: return "continue_outside_loop";
    case Id::kInvalidPattern: return "invalid_pattern";
    case Id::kInvalidSyntax: return "invalid_syntax";

    // sema (related to declaration & type)
    case Id::kUndefinedSymbol: return "undefined_symbol";
    case Id::kUndefinedVariable: return "undefined_variable";
    case Id::kUndefinedFunction: return "undefined_function";
    case Id::kUndefinedType: return "undefined_type";
    case Id::kCallArgumentMismatch: return "call_argument_mismatch";
    case Id::kReturnTypeMismatch: return "return_type_mismatch";
    case Id::kNonCallableExpression: return "non_callable_expression";
    case Id::kInvalidOperatorOperands: return "invalid_operator_operands";
    case Id::kMemberNotFound: return "member_not_found";
    case Id::kAccessPrivateMember: return "access_private_member";
    case Id::kImmutableBindingChanged: return "immutable_binding_changed";
    case Id::kConstAssignment: return "const_assignment";
    case Id::kTypeMismatch: return "type_mismatch";
    case Id::kTypeAnnotationRequired: return "type_annotation_required";
    case Id::kNonIterableExpression: return "non_iterable_expression";
    case Id::kInfiniteLoopLiteral: return "infinite_loop_literal";
    case Id::kFunctionSignatureMismatch: return "function_signature_mismatch";
    case Id::kRedeclaration: return "redeclaration";
    case Id::kConflictingDeclaration: return "conflicting_declaration";
    case Id::kConflictingTraitImplementation:
      return "conflicting_trait_implementation";
    case Id::kMissingTraitBound: return "missing_trait_bound";
    case Id::kVariableNotInitialized: return "variable_not_initialized";
    case Id::kMisplacedAttribute: return "misplaced_attribute";
    case Id::kRecursiveTypeDefinition: return "recursive_type_definition";
    case Id::kCyclicDependency: return "cyclic_dependency";

    // lifetime / borrow checker
    case Id::kDanglingReference: return "dangling_reference";
    case Id::kUnusedLifetimeParameter: return "unused_lifetime_parameter";
    case Id::kUnusedBorrow: return "unused_borrow";
    case Id::kLifetimeConflict: return "lifetime_conflict";
    case Id::kLifetimeAnnotationRequired: return "lifetime_annotation_required";
    case Id::kReturnedBorrowDoesNotLiveLongEnough:
      return "returned_borrow_does_not_live_long_enough";
    case Id::kMoveAfterBorrow: return "move_after_borrow";
    case Id::kBorrowAfterMove: return "borrow_after_move";
    case Id::kUseAfterMove: return "use_after_move";
    case Id::kMultipleMutBorrow: return "multiple_mutable_borrow";
    case Id::kMutableAlias: return "mutable_alias";
    case Id::kImmutableBorrowIntoMutable:
      return "immutable_borrow_into_mutable";

    // warnings
    case Id::kUnusedVariable: return "unused_variable";
    case Id::kUnusedFunction: return "unused_function";
    case Id::kUnreachableCode: return "unreachable_code";
    case Id::kImplicitConversion: return "implicit_conversion";
    case Id::kMissingReturnStatement: return "missing_return_statement";
    case Id::kDeprecatedFeature: return "deprecated_feature";
    case Id::kDeprecatedApiUsage: return "deprecated_api_usage";
    case Id::kAmbiguousCall: return "ambiguous_call";
    case Id::kUnnecessaryCopy: return "unnecessary_copy";
    case Id::kShadowingVariable: return "shadowing_variable";
    case Id::kNumericDivisionByZero: return "numeric_division_by_zero";
    case Id::kAlwaysTrueCondition: return "always_true_condition";
    case Id::kAlwaysFalseCondition: return "always_false_condition";
    case Id::kMissingDefaultCase: return "missing_default_case";
    case Id::kInefficientLoop: return "inefficient_loop";
    case Id::kRedundantCast: return "redundant_cast";
    case Id::kEmptyLoopBody: return "empty_loop_body";
    case Id::kIneffectiveAssignment: return "ineffective_assignment";
  }
}

// 6-character code: [prefix][digit][digit][digit][digit]\0
inline void diagnostic_id_to_code(DiagnosticId id,
                                  Severity severity,
                                  char out_buf[6]) {
  static constexpr char kSeverityPrefix[] = {'u', 'f', 'e', 'w', 'i', 'd', 't'};
  out_buf[0] = kSeverityPrefix[static_cast<uint8_t>(severity)];

  uint16_t code = static_cast<uint16_t>(id);
  out_buf[4] = '0' + (code % 10);
  code /= 10;
  out_buf[3] = '0' + (code % 10);
  code /= 10;
  out_buf[2] = '0' + (code % 10);
  code /= 10;
  out_buf[1] = '0' + (code % 10);

  out_buf[5] = '\0';
}

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_

