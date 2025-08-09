// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/base/diagnostic_engine.h"

#include <string>
#include <string_view>
#include <utility>

#include "core/base/file_manager.h"
#include "core/base/logger.h"
#include "core/base/source_range.h"
#include "frontend/diagnostic/base/diagnostic_options.h"
#include "frontend/diagnostic/data/annotation.h"
#include "frontend/diagnostic/data/diagnostic_entry.h"
#include "frontend/diagnostic/data/diagnostic_id.h"
#include "frontend/diagnostic/data/entry_builder.h"
#include "frontend/diagnostic/data/label.h"
#include "gtest/gtest.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/translator.h"

namespace diagnostic {

static unicode::Utf8FileManager file_manager;

TEST(DiagnosticEngineTest, FormatSingle) {
  std::u8string source = u8"x := 42;\ny := ;\n";
  unicode::Utf8FileId fid = file_manager.add_virtual_file(std::move(source));

  i18n::Translator translator;

  DiagnosticOptions options{
      .output_format = diagnostic::DiagnosticOutputFormat::kClassic,
      .colorize = true,
      .show_notes = true,
      .show_help = true,
      .show_source_snippet = true,
      .use_unicode = true,
  };

  DiagnosticEngine engine(&file_manager, &translator, options);

  std::string_view expected = "expression";
  std::string_view before_token = ":=";
  std::string_view assign_example = "y := 42";
  DiagnosticEntry entry =
      std::move(
          EntryBuilder(Severity::kError, DiagnosticId::kExpectedExpression)
              .label(fid, 2, 6, 1,
                     i18n::TranslationKey::kDiagnosticLabelExpectedAfter,
                     LabelMarkerType::kEmphasis, {expected, before_token})
              .annotation(AnnotationSeverity::kHelp,
                          i18n::TranslationKey::kDiagnosticAnnotationDidYouMean,
                          {assign_example}))
          .build();

  engine.push(std::move(entry));

  std::string formatted = engine.format_batch_and_clear();
  engine.clear();

  // DLOG(debug, "formatted: \n{}", formatted);

  constexpr const auto np = std::string::npos;
  EXPECT_NE(formatted.find("expected expression"), np);
  EXPECT_NE(formatted.find("y := "), np);
  EXPECT_NE(formatted.find('~'), np);
  EXPECT_NE(formatted.find("expected expression after `:=`"), np);
  EXPECT_NE(formatted.find("did you mean `y := 42`?"), np);
}

TEST(DiagnosticEngineTest, FormatMultipleLabel) {
  std::u8string source =
      u8R"(
    ref := &data;
    println#("{}", ref);
    consume(data);
  )";
  unicode::Utf8FileId fid = file_manager.add_virtual_file(std::move(source));

  i18n::Translator translator;

  DiagnosticOptions options{
      .output_format = diagnostic::DiagnosticOutputFormat::kClassic,
      .colorize = true,
      .show_notes = true,
      .show_help = true,
      .show_source_snippet = true,
      .use_unicode = true,
  };

  DiagnosticEngine engine(&file_manager, &translator, options);

  std::string_view varname = "data";
  DiagnosticEntry entry =
      std::move(
          EntryBuilder(Severity::kError,
                       DiagnosticId::kMovedVariableThatWasStillBorrowed)
              .label(fid, 2, 12, 5,
                     i18n::TranslationKey::kDiagnosticLabelBorrowOccursHere,
                     LabelMarkerType::kLine)
              .label(fid, 4, 13, 4,
                     i18n::TranslationKey::kDiagnosticLabelMoveOccursHere,
                     LabelMarkerType::kEmphasis)
              .annotation(
                  AnnotationSeverity::kNote,
                  i18n::TranslationKey::
                      kDiagnosticAnnotationMovedVariableThatWasStillBorrowed,
                  {varname})
              .annotation(
                  AnnotationSeverity::kHelp,
                  i18n::TranslationKey::kDiagnosticAnnotationTryCloningData,
                  {varname}))
          .build();

  engine.push(std::move(entry));

  std::string formatted2 = engine.format_batch_and_clear();
  engine.clear();

  // DLOG(debug, "formatted2:\n{}", formatted2);

  constexpr const auto np = std::string::npos;
  EXPECT_NE(formatted2.find("moved variable that was still borrowed"), np);
  EXPECT_NE(formatted2.find("ref := "), np);
  EXPECT_NE(formatted2.find("&data"), np);
  EXPECT_NE(formatted2.find("consume("), np);
  EXPECT_NE(formatted2.find("data"), np);
  EXPECT_NE(formatted2.find("~~~~"), np);
  EXPECT_NE(formatted2.find("borrow occurs here"), np);
  EXPECT_NE(formatted2.find("~~~~"), np);
  EXPECT_NE(formatted2.find("move occurs here"), np);
  EXPECT_NE(formatted2.find("`data` was moved while borrowed"), np);
  EXPECT_NE(formatted2.find("try cloning `data` if you need"), np);
}

TEST(DiagnosticEngineTest, FormatVeryLargeLineNumber) {
  std::u8string prefix(1000, '\n');
  std::u8string source = prefix.append(
      u8R"(
    y := ;
  )");
  unicode::Utf8FileId fid = file_manager.add_virtual_file(std::move(source));
  i18n::Translator translator;

  DiagnosticOptions options{
      .output_format = diagnostic::DiagnosticOutputFormat::kClassic,
      .colorize = true,
      .show_notes = true,
      .show_help = true,
      .show_source_snippet = true,
      .use_unicode = true,
  };

  DiagnosticEngine engine(&file_manager, &translator, options);

  std::string_view expected = "expression";
  std::string_view before_token = ":=";
  std::string_view assign_example = "y := 42";
  DiagnosticEntry entry =
      std::move(
          EntryBuilder(Severity::kError, DiagnosticId::kExpectedExpression)
              .label(fid, 1002, 10, 1,
                     i18n::TranslationKey::kDiagnosticLabelExpectedAfter,
                     LabelMarkerType::kEmphasis, {expected, before_token})
              .annotation(AnnotationSeverity::kHelp,
                          i18n::TranslationKey::kDiagnosticAnnotationDidYouMean,
                          {assign_example}))
          .build();

  engine.push(std::move(entry));

  std::string formatted3 = engine.format_batch_and_clear();
  engine.clear();

  // DLOG(debug, "formatted with very large line number: \n{}", formatted3);

  constexpr const auto np = std::string::npos;
  EXPECT_NE(formatted3.find("expected expression"), np);
  EXPECT_NE(formatted3.find("y := "), np);
  EXPECT_NE(formatted3.find('~'), np);
  EXPECT_NE(formatted3.find("expected expression after `:=`"), np);
}

}  // namespace diagnostic
