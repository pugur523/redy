// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_
#define FRONTEND_DIAGNOSTIC_DATA_DIAGNOSTIC_ID_H_

#include <cstdint>

#include "core/base/style_util.h"
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
  kUnexpectedToken = 11,
  kMissingToken = 12,
  kExpectedSemicolon = 13,
  kExpectedIdentifier = 14,
  kExpectedType = 15,
  kExpectedLeftParen = 16,
  kExpectedRightParen = 17,
  kExpectedLeftBrace = 18,
  kExpectedRightBrace = 19,
  kExpectedLeftBracket = 20,
  kExpectedRBracket = 21,
  kExpectedExpression = 22,
  kExpectedReturnExpression = 23,
  kExpectedBlock = 24,
  kUnexpectedKeyword = 25,
  kMalformedDeclaration = 26,
  kDuplicateParameterName = 27,
  kParameterCountMismatch = 28,
  kInvalidFunctionCall = 29,
  kInvalidAssignmentTarget = 30,
  kInvalidGenericArguments = 31,
  kBreakOutsideLoop = 32,
  kContinueOutsideLoop = 33,
  kInvalidPattern = 34,
  kInvalidSyntax = 35,  // fallback

  // ast analyze (symbol, type)
  kUndefinedSymbol = 36,
  kUndefinedVariable = 37,
  kUndefinedFunction = 38,
  kUndefinedType = 39,
  kCallArgumentMismatch = 40,
  kReturnTypeMismatch = 41,
  kNonCallableExpression = 42,
  kInvalidOperatorOperands = 43,
  kMemberNotFound = 44,
  kAccessPrivateMember = 45,
  kImmutableBindingChanged = 46,
  kConstAssignment = 47,
  kTypeMismatch = 48,
  kTypeAnnotationRequired = 49,
  kNonIterableExpression = 50,
  kInfiniteLoopLiteral = 51,
  kFunctionSignatureMismatch = 52,
  kRedeclaration = 53,
  kConflictingDeclaration = 54,
  kConflictingTraitImplementation = 55,
  kMissingTraitBound = 56,
  kVariableNotInitialized = 57,
  kMisplacedAttribute = 58,
  kRecursiveTypeDefinition = 59,
  kCyclicDependency = 60,
  kNumericLiteralOutOfRange = 61,

  // hir / mir analyze (lifetime infer/ borrow checker)
  kDanglingReference = 62,
  kUnusedLifetimeParameter = 63,
  kUnusedBorrow = 64,
  kLifetimeConflict = 65,
  kLifetimeAnnotationRequired = 66,
  kReturnedBorrowDoesNotLiveLongEnough = 67,
  kMovedVariableThatWasStillBorrowed = 68,
  kBorrowAfterMove = 69,
  kUseAfterMove = 70,
  kMultipleMutableBorrow = 71,
  kMutableAlias = 72,
  kImmutableBorrowIntoMutable = 73,

  // warning
  kUnusedVariable = 74,
  kUnusedFunction = 75,
  kUnreachableCode = 76,
  kImplicitConversion = 77,
  kMissingReturnStatement = 78,
  kDeprecatedFeature = 79,
  kDeprecatedApiUsage = 80,
  kAmbiguousCall = 81,
  kUnnecessaryCopy = 82,
  kShadowingVariable = 83,
  kNumericDivisionByZero = 84,
  kAlwaysTrueCondition = 85,
  kAlwaysFalseCondition = 86,
  kMissingDefaultCase = 87,
  kInefficientLoop = 88,
  kRedundantCast = 89,
  kEmptyLoopBody = 90,
  kIneffectiveAssignment = 91,
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
    case Id::kUnexpectedToken:
      return TranslationKey::kDiagnosticParserUnexpectedToken;
    case Id::kMissingToken:
      return TranslationKey::kDiagnosticParserMissingToken;
    case Id::kExpectedSemicolon:
      return TranslationKey::kDiagnosticParserExpectedSemicolon;
    case Id::kExpectedIdentifier:
      return TranslationKey::kDiagnosticParserExpectedIdentifier;
    case Id::kExpectedType:
      return TranslationKey::kDiagnosticParserExpectedType;
    case Id::kExpectedLeftParen:
      return TranslationKey::kDiagnosticParserExpectedLeftParen;
    case Id::kExpectedRightParen:
      return TranslationKey::kDiagnosticParserExpectedRightParen;
    case Id::kExpectedLeftBrace:
      return TranslationKey::kDiagnosticParserExpectedLeftBrace;
    case Id::kExpectedRightBrace:
      return TranslationKey::kDiagnosticParserExpectedRightBrace;
    case Id::kExpectedLeftBracket:
      return TranslationKey::kDiagnosticParserExpectedLeftBracket;
    case Id::kExpectedRBracket:
      return TranslationKey::kDiagnosticParserExpectedRightBracket;
    case Id::kExpectedExpression:
      return TranslationKey::kDiagnosticParserExpectedExpression;
    case Id::kExpectedReturnExpression:
      return TranslationKey::kDiagnosticParserExpectedReturnExpression;
    case Id::kExpectedBlock:
      return TranslationKey::kDiagnosticParserExpectedBlock;
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
