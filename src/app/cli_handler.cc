// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "app/cli_handler.h"

#include <iostream>
#include <string>
#include <vector>

#include "build/build_config.h"
#include "core/base/logger.h"
#include "core/cli/ansi/progress_bar.h"
#include "core/cli/ansi/style_builder.h"
#include "core/cli/ansi/style_util.h"
#include "core/cli/arg_parser.h"
#include "core/diagnostics/stack_trace.h"
#include "core/location.h"
#include "core/redy/build_type.h"
#include "core/redy/runtime_options.h"

namespace app {

namespace {

void setup_options(core::ArgParser& parser, core::RuntimeOptions* options) {
  // runtime overridable options
  parser.add_option(&options->config_file, "config",
                    "relative config file path to use", false,
                    {options->config_file});
  parser.add_alias("c", "config");

  parser.add_flag(&options->verbose, "verbose",
                  "use verbose mode for debugging or some other reasons", false,
                  {options->verbose});
  parser.add_alias("V", "verbose");

  parser.add_alias("mr", "min_size_rel");

  parser.add_positional(&options->sub_command, "sub_command",
                        "specify what to do", false);
}

}  // namespace

int handle_arguments(int argc, char** argv) {
  core::ArgParser parser(build::kBuildName, build::kBuildDescription);

  auto options = core::RuntimeOptions::create();

  setup_options(parser, options.get());

  bool debug = false;
  bool release = false;
  bool rel_w_deb_info = false;
  bool min_size_rel = false;

  // debug
  parser.add_flag(&debug, "debug", "use debug config for building", false,
                  {debug});
  parser.add_alias("d", "debug");

  // release
  parser.add_flag(&release, "release", "use release config for building", false,
                  {release});
  parser.add_alias("r", "release");

  // rel-w-deb-info
  parser.add_flag(&rel_w_deb_info, "rel_w_deb_info",
                  "use release-with-debug-info config for building", false,
                  {rel_w_deb_info});
  parser.add_alias("rd", "rel_w_deb_info");

  // min-size-rel
  parser.add_flag(&min_size_rel, "min_size_rel",
                  "use minimum-size-release config for building", false,
                  {min_size_rel});

  auto result = parser.parse(argc, argv);
  if (result != core::ParseResult::kSuccess) {
    return static_cast<int>(result);
  }

  if (debug) {
    options->build_type = core::BuildType::kDebug;
  } else if (release) {
    options->build_type = core::BuildType::kRelease;
  } else if (rel_w_deb_info) {
    options->build_type = core::BuildType::kReleaseWithDebugInfo;
  } else if (min_size_rel) {
    options->build_type = core::BuildType::kMinimumSizeRelease;
  } else {
    options->build_type = core::BuildType::kDebug;
  }

  if (options->verbose) {
    core::glog.raw<"parsed arguments:\n  {}">(options->to_string(2));
  }

  core::StyleBuilder ing;
  ing.style(core::Style::kBoldUnderline).colour(core::Colour::kBrightBlue);
  core::ProgressBar bar(30, ing.build("Compiling...") + " : demo.ry");

  core::StyleBuilder ed;
  ed.style(core::Style::kBoldUnderline).colour(core::Colour::kBrightGreen);
  for (int i = 0; i <= 10000; ++i) {
    core::glog.raw<"\r{}">(bar.update(i / 10000.0));
    std::this_thread::sleep_for(std::chrono::microseconds(10));
  }
  core::glog.raw<"\r{}\n">(bar.finish(ed.build("Completed!") + "   : demo.ry"));

  return 0;
}

}  // namespace app
