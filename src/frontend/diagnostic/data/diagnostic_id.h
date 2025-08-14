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
  kMissingToken = 12,
  kUnexpectedToken = 13,
  kExpectedButFound = 14,
  kUnexpectedKeyword = 15,
  kMalformedDeclaration = 16,
  kDuplicateParameterName = 17,
  kParameterCountMismatch = 18,
  kInvalidFunctionCall = 19,
  kInvalidAssignmentTarget = 20,
  kInvalidGenericArguments = 21,
  kBreakOutsideLoop = 22,
  kContinueOutsideLoop = 23,
  kInvalidPattern = 24,
  kInvalidSyntax = 25,  // fallback

  // ast analyze (symbol, type)
  kUndefinedSymbol = 26,
  kUndefinedVariable = 27,
  kUndefinedFunction = 28,
  kUndefinedType = 29,
  kCallArgumentMismatch = 30,
  kReturnTypeMismatch = 31,
  kNonCallableExpression = 32,
  kInvalidOperatorOperands = 33,
  kMemberNotFound = 34,
  kAccessPrivateMember = 35,
  kImmutableBindingChanged = 36,
  kConstAssignment = 37,
  kTypeMismatch = 38,
  kTypeAnnotationRequired = 39,
  kNonIterableExpression = 40,
  kInfiniteLoopLiteral = 41,
  kFunctionSignatureMismatch = 42,
  kRedeclaration = 43,
  kConflictingDeclaration = 44,
  kConflictingTraitImplementation = 45,
  kMissingTraitBound = 46,
  kVariableNotInitialized = 47,
  kMisplacedAttribute = 48,
  kRecursiveTypeDefinition = 49,
  kCyclicDependency = 50,
  kNumericLiteralOutOfRange = 51,

  // hir / mir analyze (lifetime infer/ borrow checker)
  kDanglingReference = 52,
  kUnusedLifetimeParameter = 53,
  kUnusedBorrow = 54,
  kLifetimeConflict = 55,
  kLifetimeAnnotationRequired = 56,
  kReturnedBorrowDoesNotLiveLongEnough = 57,
  kMovedVariableThatWasStillBorrowed = 58,
  kBorrowAfterMove = 59,
  kUseAfterMove = 60,
  kMultipleMutableBorrow = 61,
  kMutableAlias = 62,
  kImmutableBorrowIntoMutable = 63,

  // warning
  kUnusedVariable = 64,
  kUnusedFunction = 65,
  kUnreachableCode = 66,
  kImplicitConversion = 67,
  kMissingReturnStatement = 68,
  kDeprecatedFeature = 69,
  kDeprecatedApiUsage = 70,
  kAmbiguousCall = 71,
  kUnnecessaryCopy = 72,
  kShadowingVariable = 73,
  kNumericDivisionByZero = 74,
  kAlwaysTrueCondition = 75,
  kAlwaysFalseCondition = 76,
  kMissingDefaultCase = 77,
  kInefficientLoop = 78,
  kRedundantCast = 79,
  kEmptyLoopBody = 80,
  kIneffectiveAssignment = 81,
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
