// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "frontend/diagnostic/base/diagnostic_engine.h"

#include <string>
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

namespace diagnostic {

static core::FileManager file_manager;

TEST(DiagnosticEngineTest, FormatSingle) {
  std::string source = "x := 42;\ny := ;\n";
  core::FileId fid = file_manager.add_virtual_file(std::move(source));

  DiagnosticOptions options{
      .output_format = diagnostic::DiagnosticOutputFormat::kClassic,
      .colorize = true,
      .show_notes = true,
      .show_help = true,
      .show_source_snippet = true,
      .use_unicode = true,
  };

  DiagnosticEngine engine(&file_manager, options);

  DiagnosticEntry entry =
      EntryBuilder(Severity::kError, DiagnosticId::kExpectedExpression,
                   "expected expression")
          .label(fid, 2, 6, 1, "expected expression after ':='")
          .annotation("did you mean `y := 57`?", AnnotationKind::kHelp)
          .build();

  engine.push(std::move(entry));

  std::string formatted = engine.format_batch();
  engine.clear();

  DLOG(debug, "format single: \n{}", formatted);

  constexpr const auto np = std::string::npos;
  EXPECT_NE(formatted.find("expected expression"), np);
  EXPECT_NE(formatted.find("y := ;"), np);
  EXPECT_NE(formatted.find('^'), np);
  EXPECT_NE(formatted.find("expected expression after ':='"), np);
}

TEST(DiagnosticEngineTest, FormatMultipleLabel) {
  std::string source = R"(
    ref := &data;
    print("{}", ref);
    consume(data);
  )";
  core::FileId fid = file_manager.add_virtual_file(std::move(source));

  DiagnosticOptions options{
      .output_format = diagnostic::DiagnosticOutputFormat::kClassic,
      .colorize = true,
      .show_notes = true,
      .show_help = true,
      .show_source_snippet = true,
      .use_unicode = true,
  };

  DiagnosticEngine engine(&file_manager, options);

  DiagnosticEntry entry =
      EntryBuilder(Severity::kError, DiagnosticId::kMoveAfterBorrow,
                   "value moved after borrow")
          .label(fid, 2, 12, 5, "borrow occurs here")
          .label(fid, 4, 13, 4, "move occurs here")
          .annotation("`data` was borrowed, but then moved",
                      AnnotationKind::kNote)
          .annotation("try cloning `data` if you need to use it after moving",
                      AnnotationKind::kHelp)
          .build();

  engine.push(std::move(entry));

  std::string formatted2 = engine.format_batch();
  engine.clear();

  DLOG(debug, "format multiple label: \n{}", formatted2);

  constexpr const auto np = std::string::npos;
  EXPECT_NE(formatted2.find("value moved after borrow"), np);
  EXPECT_NE(formatted2.find("ref := &data;"), np);
  EXPECT_NE(formatted2.find("consume(data);"), np);
  EXPECT_NE(formatted2.find("^~~~"), np);
  EXPECT_NE(formatted2.find("move occurs here"), np);
  EXPECT_NE(formatted2.find("`data` was borrowed, but then moved"), np);
  EXPECT_NE(formatted2.find("try cloning `data` if you need"), np);
}

TEST(DiagnosticEngineTest, FormatVeryLargeLineNumber) {
  std::string prefix(1000, '\n');
  std::string source = prefix.append(R"(
    y := ;
  )");
  core::FileId fid = file_manager.add_virtual_file(std::move(source));

  DiagnosticOptions options{
      .output_format = diagnostic::DiagnosticOutputFormat::kClassic,
      .colorize = true,
      .show_notes = true,
      .show_help = true,
      .show_source_snippet = true,
      .use_unicode = true,
  };

  DiagnosticEngine engine(&file_manager, options);

  DiagnosticEntry entry =
      EntryBuilder(Severity::kError, DiagnosticId::kExpectedExpression,
                   "expected expression")
          .label(fid, 1002, 10, 1, "expected expression after ':='")
          .annotation("did you mean `y := 42`?", AnnotationKind::kHelp)
          .build();

  engine.push(std::move(entry));

  std::string formatted3 = engine.format_batch();
  engine.clear();

  DLOG(debug, "format very large line number: \n{}", formatted3);

  constexpr const auto np = std::string::npos;
  EXPECT_NE(formatted3.find("expected expression"), np);
  EXPECT_NE(formatted3.find("y := ;"), np);
  EXPECT_NE(formatted3.find('^'), np);
  EXPECT_NE(formatted3.find("expected expression after ':='"), np);
}

}  // namespace diagnostic
