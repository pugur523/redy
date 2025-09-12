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
//   kResolve = 3,
//   kHirAnalyze = 4,
//   kMirAnalyze = 5,
//   kCodegen = 6,
// };

enum class DiagnosticId : uint8_t {
  kUnknown = 0,

  // ok
  kOk = 1,

  // generic
  kFileNotFound = 2,

  // lexer
  kInvalidUtfSequence = 3,
  kUnterminatedStringLiteral = 4,
  kUnterminatedCharacterLiteral = 5,
  kUnterminatedBlockComment = 6,
  kUnrecognizedCharacter = 7,
  kInvalidCharacterEscape = 8,
  kInvalidOctalEscape = 9,
  kInvalidHexEscape = 10,
  kInvalidUnicodeEscape = 11,
  kInvalidNumericLiteral = 12,
  kUnexpectedEndOfFile = 13,

  // parser
  kInvalidToken = 14,
  kMissingToken = 15,
  kUnexpectedToken = 16,
  kExpectedButFound = 17,
  kCannotBePostfixOperator = 18,
  kConflictingStorageSpecifiers = 19,
  kInvalidSyntax = 20,  // fallback

  // resolver
  kUndefinedSymbol = 21,
  kUndefinedVariable = 22,
  kUndefinedFunction = 23,
  kUndefinedType = 24,
  kMalformedDeclaration = 25,
  kDuplicateParameterName = 26,
  kParameterCountMismatch = 27,
  kInvalidFunctionCall = 28,
  kInvalidAssignmentTarget = 29,
  kInvalidGenericArguments = 30,
  kBreakOutsideLoop = 31,
  kContinueOutsideLoop = 32,
  kInvalidPattern = 33,
  kCallArgumentMismatch = 34,
  kReturnTypeMismatch = 35,
  kNonCallableExpression = 36,
  kInvalidOperatorOperands = 37,
  kMemberNotFound = 38,
  kAccessPrivateMember = 39,
  kImmutableBindingChanged = 40,
  kConstAssignment = 41,
  kTypeMismatch = 42,
  kTypeAnnotationRequired = 43,
  kNonIterableExpression = 44,
  kInfiniteLoopLiteral = 45,
  kFunctionSignatureMismatch = 46,
  kRedeclaration = 47,
  kConflictingDeclaration = 48,
  kConflictingTraitImplementation = 49,
  kMissingTraitBound = 50,
  kVariableNotInitialized = 51,
  kMisplacedAttribute = 52,
  kRecursiveTypeDefinition = 53,
  kCyclicDependency = 54,
  kNumericLiteralOutOfRange = 55,

  // hir / mir analyze (lifetime infer/ borrow checker)
  kDanglingReference = 56,
  kUnusedLifetimeParameter = 57,
  kUnusedBorrow = 58,
  kLifetimeConflict = 59,
  kLifetimeAnnotationRequired = 60,
  kReturnedBorrowDoesNotLiveLongEnough = 61,
  kMovedVariableThatWasStillBorrowed = 62,
  kBorrowAfterMove = 63,
  kUseAfterMove = 64,
  kMultipleMutableBorrow = 65,
  kMutableAlias = 66,
  kImmutableBorrowIntoMutable = 67,

  // warning
  kUnusedVariable = 68,
  kUnusedFunction = 69,
  kUnreachableCode = 70,
  kImplicitConversion = 71,
  kMissingReturnStatement = 72,
  kDeprecatedFeature = 73,
  kDeprecatedApiUsage = 74,
  kAmbiguousCall = 75,
  kUnnecessaryCopy = 76,
  kShadowingVariable = 77,
  kNumericDivisionByZero = 78,
  kAlwaysTrueCondition = 79,
  kAlwaysFalseCondition = 80,
  kMissingDefaultCase = 81,
  kInefficientLoop = 82,
  kRedundantCast = 83,
  kEmptyLoopBody = 84,
  kIneffectiveAssignment = 85,
};

inline constexpr i18n::TranslationKey diagnostic_id_to_tr_key(DiagnosticId id) {
  using Id = DiagnosticId;
  using TranslationKey = i18n::TranslationKey;
  switch (id) {
    case Id::kUnknown: return TranslationKey::kDiagnosticUnknown;

    // ok
    case Id::kOk: return TranslationKey::kDiagnosticOk;

    // generic
    case Id::kFileNotFound:
      return TranslationKey::kDiagnosticGenericFileNotFound;

    // lexer
    case Id::kInvalidUtfSequence:
      return TranslationKey::kDiagnosticLexerInvalidUtfSequence;
    case Id::kUnterminatedStringLiteral:
      return TranslationKey::kDiagnosticLexerUnterminatedStringLiteral;
    case Id::kUnterminatedCharacterLiteral:
      return TranslationKey::kDiagnosticLexerUnterminatedCharacterLiteral;
    case Id::kUnterminatedBlockComment:
      return TranslationKey::kDiagnosticLexerUnterminatedBlockComment;
    case Id::kUnrecognizedCharacter:
      return TranslationKey::kDiagnosticLexerUnrecognizedCharacter;
    case Id::kInvalidCharacterEscape:
      return TranslationKey::kDiagnosticLexerInvalidCharacterEscape;
    case Id::kInvalidOctalEscape:
      return TranslationKey::kDiagnosticLexerInvalidOctalEscape;
    case Id::kInvalidHexEscape:
      return TranslationKey::kDiagnosticLexerInvalidHexEscape;
    case Id::kInvalidUnicodeEscape:
      return TranslationKey::kDiagnosticLexerInvalidUnicodeEscape;
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
    case Id::kConflictingStorageSpecifiers:
      return TranslationKey::kDiagnosticParserConflictingStorageSpecifiers;
    case Id::kInvalidSyntax:
      return TranslationKey::kDiagnosticParserInvalidSyntax;

    // resolver
    case Id::kUndefinedSymbol:
      return TranslationKey::kDiagnosticResolverUndefinedSymbol;
    case Id::kUndefinedVariable:
      return TranslationKey::kDiagnosticResolverUndefinedVariable;
    case Id::kUndefinedFunction:
      return TranslationKey::kDiagnosticResolverUndefinedFunction;
    case Id::kUndefinedType:
      return TranslationKey::kDiagnosticResolverUndefinedType;
    case Id::kMalformedDeclaration:
      return TranslationKey::kDiagnosticResolverMalformedDeclaration;
    case Id::kDuplicateParameterName:
      return TranslationKey::kDiagnosticResolverDuplicateParameterName;
    case Id::kParameterCountMismatch:
      return TranslationKey::kDiagnosticResolverParameterCountMismatch;
    case Id::kInvalidFunctionCall:
      return TranslationKey::kDiagnosticResolverInvalidFunctionCall;
    case Id::kInvalidAssignmentTarget:
      return TranslationKey::kDiagnosticResolverInvalidAssignmentTarget;
    case Id::kInvalidGenericArguments:
      return TranslationKey::kDiagnosticResolverInvalidGenericArguments;
    case Id::kBreakOutsideLoop:
      return TranslationKey::kDiagnosticResolverBreakOutsideLoop;
    case Id::kContinueOutsideLoop:
      return TranslationKey::kDiagnosticResolverContinueOutsideLoop;
    case Id::kInvalidPattern:
      return TranslationKey::kDiagnosticResolverInvalidPattern;
    case Id::kCallArgumentMismatch:
      return TranslationKey::kDiagnosticResolverCallArgumentMismatch;
    case Id::kReturnTypeMismatch:
      return TranslationKey::kDiagnosticResolverReturnTypeMismatch;
    case Id::kNonCallableExpression:
      return TranslationKey::kDiagnosticResolverNonCallableExpression;
    case Id::kInvalidOperatorOperands:
      return TranslationKey::kDiagnosticResolverInvalidOperatorOperands;
    case Id::kMemberNotFound:
      return TranslationKey::kDiagnosticResolverMemberNotFound;
    case Id::kAccessPrivateMember:
      return TranslationKey::kDiagnosticResolverAccessPrivateMember;
    case Id::kImmutableBindingChanged:
      return TranslationKey::kDiagnosticResolverImmutableBindingChanged;
    case Id::kConstAssignment:
      return TranslationKey::kDiagnosticResolverConstAssignment;
    case Id::kTypeMismatch:
      return TranslationKey::kDiagnosticResolverTypeMismatch;
    case Id::kTypeAnnotationRequired:
      return TranslationKey::kDiagnosticResolverTypeAnnotationRequired;
    case Id::kNonIterableExpression:
      return TranslationKey::kDiagnosticResolverNonIterableExpression;
    case Id::kInfiniteLoopLiteral:
      return TranslationKey::kDiagnosticResolverInfiniteLoopLiteral;
    case Id::kFunctionSignatureMismatch:
      return TranslationKey::kDiagnosticResolverFunctionSignatureMismatch;
    case Id::kRedeclaration:
      return TranslationKey::kDiagnosticResolverRedeclaration;
    case Id::kConflictingDeclaration:
      return TranslationKey::kDiagnosticResolverConflictingDeclaration;
    case Id::kConflictingTraitImplementation:
      return TranslationKey::kDiagnosticResolverConflictingTraitImplementation;
    case Id::kMissingTraitBound:
      return TranslationKey::kDiagnosticResolverMissingTraitBound;
    case Id::kVariableNotInitialized:
      return TranslationKey::kDiagnosticResolverVariableNotInitialized;
    case Id::kMisplacedAttribute:
      return TranslationKey::kDiagnosticResolverMisplacedAttribute;
    case Id::kRecursiveTypeDefinition:
      return TranslationKey::kDiagnosticResolverRecursiveTypeDefinition;
    case Id::kCyclicDependency:
      return TranslationKey::kDiagnosticResolverCyclicDependency;
    case Id::kNumericLiteralOutOfRange:
      return TranslationKey::kDiagnosticResolverNumericLiteralOutOfRange;

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
