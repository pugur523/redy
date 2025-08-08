// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#include "i18n/base/translator.h"

#include "gtest/gtest.h"
#include "i18n/base/data/language_id.h"

namespace i18n {

TEST(TranslatorTest, PrimaryLanguageOnly) {
  Translator tr(TranslatorConfig::create_default(LanguageId::kJaJp));
  EXPECT_EQ(tr.translate<TranslationKey::kCompilationSuccess>(),
            "コンパイル成功");
}

TEST(TranslatorTest, FallbackToEnglish) {
  Translator tr(
      TranslatorConfig::with_fallbacks(LanguageId::kEnUk, {
                                                              LanguageId::kEnUs,
                                                              LanguageId::kJaJp,
                                                          }));
  EXPECT_EQ(tr.translate<TranslationKey::kCompilationSuccess>(),
            "compilation successful");
}

TEST(TranslatorTest, KeyAsString) {
  Translator tr(TranslatorConfig::create_default(LanguageId::kEnUs));
  EXPECT_EQ((tr.translate<"compilation.success">()), "compilation successful");
}

}  // namespace i18n
