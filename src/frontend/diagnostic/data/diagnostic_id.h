// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_
#define FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_

#include <cstdint>

#include "core/cli/ansi/style_util.h"
#include "i18n/base/data/translation_key.h"

namespace diagnostic {

// enum class Phase : uint8_t {
//   kGeneric = 0,
//   kTokenize = 1,
//   kParse = 2,
//   kAstAnalyze = 3,
//   kHirAnalyze = 4,
//   kMirAnalyze = 5,
//   kCodegen = 6,
// };

enum class DiagnosticId : uint8_t {
  kUnknown = 0,

  // ok
  kOk = 1,

  // lexer
  kUnterminatedStringLiteral = 2,
  kUnterminatedCharacterLiteral = 3,
  kUnterminatedBlockComment = 4,
  kUnrecognizedCharacter = 5,
  kInvalidEscapeSequence = 6,
  kInvalidUtfSequence = 7,
  kInvalidNumericLiteral = 8,
  kUnexpectedEndOfFile = 9,

  // parser
  kInvalidToken = 10,
  kMissingToken = 11,
  kUnexpectedToken = 12,
  kExpectedButFound = 13,
  kCannotBePostfixOperator = 14,
  kUnexpectedKeyword = 15,
  kMalformedDeclaration = 16,
  kDuplicateParameterName = 17,
  kParameterCountMismatch = 18,
  kInvalidFunctionCall = 19,
  kInvalidAssignmentTarget = 20,
  kInvalidGenericArguments = 21,
  kBreakOutsideLoop = 22,
  kContinueOutsideLoop = 23,
  kConflictingStorageSpecifiers = 24,
  kInvalidPattern = 25,
  kInvalidSyntax = 26,  // fallback

  // ast analyze (symbol, type)
  kUndefinedSymbol = 27,
  kUndefinedVariable = 28,
  kUndefinedFunction = 29,
  kUndefinedType = 30,
  kCallArgumentMismatch = 31,
  kReturnTypeMismatch = 32,
  kNonCallableExpression = 33,
  kInvalidOperatorOperands = 34,
  kMemberNotFound = 35,
  kAccessPrivateMember = 36,
  kImmutableBindingChanged = 37,
  kConstAssignment = 38,
  kTypeMismatch = 39,
  kTypeAnnotationRequired = 40,
  kNonIterableExpression = 41,
  kInfiniteLoopLiteral = 42,
  kFunctionSignatureMismatch = 43,
  kRedeclaration = 44,
  kConflictingDeclaration = 45,
  kConflictingTraitImplementation = 46,
  kMissingTraitBound = 47,
  kVariableNotInitialized = 48,
  kMisplacedAttribute = 49,
  kRecursiveTypeDefinition = 50,
  kCyclicDependency = 51,
  kNumericLiteralOutOfRange = 52,

  // hir / mir analyze (lifetime infer/ borrow checker)
  kDanglingReference = 53,
  kUnusedLifetimeParameter = 54,
  kUnusedBorrow = 55,
  kLifetimeConflict = 56,
  kLifetimeAnnotationRequired = 57,
  kReturnedBorrowDoesNotLiveLongEnough = 58,
  kMovedVariableThatWasStillBorrowed = 59,
  kBorrowAfterMove = 60,
  kUseAfterMove = 61,
  kMultipleMutableBorrow = 62,
  kMutableAlias = 63,
  kImmutableBorrowIntoMutable = 64,

  // warning
  kUnusedVariable = 65,
  kUnusedFunction = 66,
  kUnreachableCode = 67,
  kImplicitConversion = 68,
  kMissingReturnStatement = 69,
  kDeprecatedFeature = 70,
  kDeprecatedApiUsage = 71,
  kAmbiguousCall = 72,
  kUnnecessaryCopy = 73,
  kShadowingVariable = 74,
  kNumericDivisionByZero = 75,
  kAlwaysTrueCondition = 76,
  kAlwaysFalseCondition = 77,
  kMissingDefaultCase = 78,
  kInefficientLoop = 79,
  kRedundantCast = 80,
  kEmptyLoopBody = 81,
  kIneffectiveAssignment = 82,
};

inline constexpr i18n::TranslationKey diagnostic_id_to_tr_key(DiagnosticId id) {
  using Id = DiagnosticId;
  using TranslationKey = i18n::TranslationKey;
  switch (id) {
    case Id::kUnknown: return TranslationKey::kDiagnosticUnknown;

    // ok
    case Id::kOk: return TranslationKey::kDiagnosticOk;

    // lexer
    case Id::kUnterminatedStringLiteral:
      return TranslationKey::kDiagnosticLexerUnterminatedStringLiteral;
    case Id::kUnterminatedCharacterLiteral:
      return TranslationKey::kDiagnosticLexerUnterminatedCharacterLiteral;
    case Id::kUnterminatedBlockComment:
      return TranslationKey::kDiagnosticLexerUnterminatedBlockComment;
    case Id::kUnrecognizedCharacter:
      return TranslationKey::kDiagnosticLexerUnrecognizedCharacter;
    case Id::kInvalidEscapeSequence:
      return TranslationKey::kDiagnosticLexerInvalidEscapeSequence;
    case Id::kInvalidUtfSequence:
      return TranslationKey::kDiagnosticLexerInvalidUtfSequence;
    case Id::kInvalidNumericLiteral:
      return TranslationKey::kDiagnosticLexerInvalidNumericLiteral;
    case Id::kUnexpectedEndOfFile:
      return TranslationKey::kDiagnosticLexerUnexpectedEndOfFile;

    // parser
    case Id::kInvalidToken:
      return TranslationKey::kDiagnosticParserInvalidToken;
    case Id::kMissingToken:
      return TranslationKey::kDiagnosticParserMissingToken;
    case Id::kUnexpectedToken:
      return TranslationKey::kDiagnosticParserUnexpectedToken;
    case Id::kExpectedButFound:
      return TranslationKey::kDiagnosticParserExpectedButFound;
    case Id::kCannotBePostfixOperator:
      return TranslationKey::kDiagnosticParserCannotBePostfixOperator;
    case Id::kUnexpectedKeyword:
      return TranslationKey::kDiagnosticParserUnexpectedKeyword;
    case Id::kMalformedDeclaration:
      return TranslationKey::kDiagnosticParserMalformedDeclaration;
    case Id::kDuplicateParameterName:
      return TranslationKey::kDiagnosticParserDuplicateParameterName;
    case Id::kParameterCountMismatch:
      return TranslationKey::kDiagnosticParserParameterCountMismatch;
    case Id::kInvalidFunctionCall:
      return TranslationKey::kDiagnosticParserInvalidFunctionCall;
    case Id::kInvalidAssignmentTarget:
      return TranslationKey::kDiagnosticParserInvalidAssignmentTarget;
    case Id::kInvalidGenericArguments:
      return TranslationKey::kDiagnosticParserInvalidGenericArguments;
    case Id::kBreakOutsideLoop:
      return TranslationKey::kDiagnosticParserBreakOutsideLoop;
    case Id::kContinueOutsideLoop:
      return TranslationKey::kDiagnosticParserContinueOutsideLoop;
    case Id::kInvalidPattern:
      return TranslationKey::kDiagnosticParserInvalidPattern;
    case Id::kConflictingStorageSpecifiers:
      return TranslationKey::kDiagnosticParserConflictingStorageSpecifiers;
    case Id::kInvalidSyntax:
      return TranslationKey::kDiagnosticParserInvalidSyntax;

    // ast analyze (symbol, type)
    case Id::kUndefinedSymbol:
      return TranslationKey::kDiagnosticAstAnalyzeUndefinedSymbol;
    case Id::kUndefinedVariable:
      return TranslationKey::kDiagnosticAstAnalyzeUndefinedVariable;
    case Id::kUndefinedFunction:
      return TranslationKey::kDiagnosticAstAnalyzeUndefinedFunction;
    case Id::kUndefinedType:
      return TranslationKey::kDiagnosticAstAnalyzeUndefinedType;
    case Id::kCallArgumentMismatch:
      return TranslationKey::kDiagnosticAstAnalyzeCallArgumentMismatch;
    case Id::kReturnTypeMismatch:
      return TranslationKey::kDiagnosticAstAnalyzeReturnTypeMismatch;
    case Id::kNonCallableExpression:
      return TranslationKey::kDiagnosticAstAnalyzeNonCallableExpression;
    case Id::kInvalidOperatorOperands:
      return TranslationKey::kDiagnosticAstAnalyzeInvalidOperatorOperands;
    case Id::kMemberNotFound:
      return TranslationKey::kDiagnosticAstAnalyzeMemberNotFound;
    case Id::kAccessPrivateMember:
      return TranslationKey::kDiagnosticAstAnalyzeAccessPrivateMember;
    case Id::kImmutableBindingChanged:
      return TranslationKey::kDiagnosticAstAnalyzeImmutableBindingChanged;
    case Id::kConstAssignment:
      return TranslationKey::kDiagnosticAstAnalyzeConstAssignment;
    case Id::kTypeMismatch:
      return TranslationKey::kDiagnosticAstAnalyzeTypeMismatch;
    case Id::kTypeAnnotationRequired:
      return TranslationKey::kDiagnosticAstAnalyzeTypeAnnotationRequired;
    case Id::kNonIterableExpression:
      return TranslationKey::kDiagnosticAstAnalyzeNonIterableExpression;
    case Id::kInfiniteLoopLiteral:
      return TranslationKey::kDiagnosticAstAnalyzeInfiniteLoopLiteral;
    case Id::kFunctionSignatureMismatch:
      return TranslationKey::kDiagnosticAstAnalyzeFunctionSignatureMismatch;
    case Id::kRedeclaration:
      return TranslationKey::kDiagnosticAstAnalyzeRedeclaration;
    case Id::kConflictingDeclaration:
      return TranslationKey::kDiagnosticAstAnalyzeConflictingDeclaration;
    case Id::kConflictingTraitImplementation:
      return TranslationKey::
          kDiagnosticAstAnalyzeConflictingTraitImplementation;
    case Id::kMissingTraitBound:
      return TranslationKey::kDiagnosticAstAnalyzeMissingTraitBound;
    case Id::kVariableNotInitialized:
      return TranslationKey::kDiagnosticAstAnalyzeVariableNotInitialized;
    case Id::kMisplacedAttribute:
      return TranslationKey::kDiagnosticAstAnalyzeMisplacedAttribute;
    case Id::kRecursiveTypeDefinition:
      return TranslationKey::kDiagnosticAstAnalyzeRecursiveTypeDefinition;
    case Id::kCyclicDependency:
      return TranslationKey::kDiagnosticAstAnalyzeCyclicDependency;
    case Id::kNumericLiteralOutOfRange:
      return TranslationKey::kDiagnosticAstAnalyzeNumericLiteralOutOfRange;

    // hir / mir analyze (lifetime infer/ borrow checker)
    case Id::kDanglingReference:
      return TranslationKey::kDiagnosticIrAnalyzeDanglingReference;
    case Id::kUnusedLifetimeParameter:
      return TranslationKey::kDiagnosticIrAnalyzeUnusedLifetimeParameter;
    case Id::kUnusedBorrow:
      return TranslationKey::kDiagnosticIrAnalyzeUnusedBorrow;
    case Id::kLifetimeConflict:
      return TranslationKey::kDiagnosticIrAnalyzeLifetimeConflict;
    case Id::kLifetimeAnnotationRequired:
      return TranslationKey::kDiagnosticIrAnalyzeLifetimeAnnotationRequired;
    case Id::kReturnedBorrowDoesNotLiveLongEnough:
      return TranslationKey::
          kDiagnosticIrAnalyzeReturnedBorrowDoesNotLiveLongEnough;
    case Id::kMovedVariableThatWasStillBorrowed:
      return TranslationKey::
          kDiagnosticIrAnalyzeMovedVariableThatWasStillBorrowed;
    case Id::kBorrowAfterMove:
      return TranslationKey::kDiagnosticIrAnalyzeBorrowAfterMove;
    case Id::kUseAfterMove:
      return TranslationKey::kDiagnosticIrAnalyzeUseAfterMove;
    case Id::kMultipleMutableBorrow:
      return TranslationKey::kDiagnosticIrAnalyzeMultipleMutableBorrow;
    case Id::kMutableAlias:
      return TranslationKey::kDiagnosticIrAnalyzeMutableAlias;
    case Id::kImmutableBorrowIntoMutable:
      return TranslationKey::kDiagnosticIrAnalyzeImmutableBorrowIntoMutable;

    // warning
    case Id::kUnusedVariable:
      return TranslationKey::kDiagnosticWarningUnusedVariable;
    case Id::kUnusedFunction:
      return TranslationKey::kDiagnosticWarningUnusedFunction;
    case Id::kUnreachableCode:
      return TranslationKey::kDiagnosticWarningUnreachableCode;
    case Id::kImplicitConversion:
      return TranslationKey::kDiagnosticWarningImplicitConversion;
    case Id::kMissingReturnStatement:
      return TranslationKey::kDiagnosticWarningMissingReturnStatement;
    case Id::kDeprecatedFeature:
      return TranslationKey::kDiagnosticWarningDeprecatedFeature;
    case Id::kDeprecatedApiUsage:
      return TranslationKey::kDiagnosticWarningDeprecatedApiUsage;
    case Id::kAmbiguousCall:
      return TranslationKey::kDiagnosticWarningAmbiguousCall;
    case Id::kUnnecessaryCopy:
      return TranslationKey::kDiagnosticWarningUnnecessaryCopy;
    case Id::kShadowingVariable:
      return TranslationKey::kDiagnosticWarningShadowingVariable;
    case Id::kNumericDivisionByZero:
      return TranslationKey::kDiagnosticWarningNumericDivisionByZero;
    case Id::kAlwaysTrueCondition:
      return TranslationKey::kDiagnosticWarningAlwaysTrueCondition;
    case Id::kAlwaysFalseCondition:
      return TranslationKey::kDiagnosticWarningAlwaysFalseCondition;
    case Id::kMissingDefaultCase:
      return TranslationKey::kDiagnosticWarningMissingDefaultCase;
    case Id::kInefficientLoop:
      return TranslationKey::kDiagnosticWarningInefficientLoop;
    case Id::kRedundantCast:
      return TranslationKey::kDiagnosticWarningRedundantCast;
    case Id::kEmptyLoopBody:
      return TranslationKey::kDiagnosticWarningEmptyLoopBody;
    case Id::kIneffectiveAssignment:
      return TranslationKey::kDiagnosticWarningIneffectiveAssignment;
  }
}

enum class Severity : uint8_t;

// 6-character code: [prefix][digit][digit][digit][digit]\0
inline constexpr void diagnostic_id_to_code(DiagnosticId id,
                                            Severity severity,
                                            char out_buf[6]) {
  constexpr char kSeverityPrefix[] = {'?', 'f', 'e', 'w', 'i', 'd', 't'};
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

using DiagId = DiagnosticId;

}  // namespace diagnostic

#endif  // FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_
