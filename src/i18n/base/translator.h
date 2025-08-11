// Copyright 2025 pugur
// This source code is licensed under the Apache License, Version 2.0
// which can be found in the LICENSE file.

#ifndef I18N_BASE_TRANSLATOR_H_
#define I18N_BASE_TRANSLATOR_H_

#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <span>
#include <string>
#include <utility>

#include "core/check.h"
#include "i18n/base/data/language_id.h"
#include "i18n/base/data/translation_impl.h"
#include "i18n/base/data/translation_key.h"
#include "i18n/base/i18n_export.h"
#include "i18n/base/translation_util.h"

namespace i18n {

struct TranslatorConfig {
  LanguageId primary_language = LanguageId::kDefault;

  // fallback languages in priority order (max 4 fallbacks for efficiency)
  std::array<LanguageId, 4> fallback_languages{};
  std::size_t fallback_count = 0;

  // whether to check if string is empty and continue fallback chain
  bool skip_empty_strings = true;

  static inline constexpr TranslatorConfig create_default(
      LanguageId lang = LanguageId::kDefault) {
    return {lang, {}, 0, true};
  }

  template <std::size_t N>
  static inline constexpr TranslatorConfig with_fallbacks(
      LanguageId primary,
      const LanguageId (&fallbacks)[N],
      bool skip_empty = true) {
    static_assert(N <= 4, "too many fallback languages (max 4)");
    TranslatorConfig config{primary, {}, 0, skip_empty};
    for (std::size_t i = 0; i < N; ++i) {
      config.fallback_languages[i] = fallbacks[i];
    }
    config.fallback_count = N;
    return config;
  }

  // common preset: regional -> language -> english
  // e.g., ja_jp -> fr_fr -> en_uk
  static inline constexpr TranslatorConfig regional(LanguageId regional_lang) {
    // this would need to be expanded based on your actual language mappings
    // example for japanese:
    if (regional_lang == LanguageId::kJaJp) {
      constexpr LanguageId fallbacks[] = {LanguageId::kJaJp, LanguageId::kEnUs,
                                          LanguageId::kDefault};
      TranslatorConfig::with_fallbacks(regional_lang, fallbacks);
    }
    // Add more regional mappings as needed
    return create_default(regional_lang);
  }
};

class I18N_EXPORT Translator {
 public:
  explicit Translator(
      const TranslatorConfig& config = TranslatorConfig::create_default())
      : config_(config) {}

  ~Translator() = default;

  Translator(const Translator&) = delete;
  Translator& operator=(const Translator&) = delete;

  Translator(Translator&&) = default;
  Translator& operator=(Translator&&) = default;

  // update the translator configuration
  inline void config(const TranslatorConfig& config) { config_ = config; }

  inline const TranslatorConfig& config() const { return config_; }

  // convenience method to change just the primary language
  inline void set_language(LanguageId lang_id) {
    config_.primary_language = lang_id;
  }

  // main translation method with fallback chain support
  template <TranslationKey key_enum>
  inline constexpr const char* translate() const {
    constexpr std::size_t kKeyIndex = static_cast<std::size_t>(key_enum);
    static_assert(kKeyIndex < kTranslationKeyCount, "invalid translation key");
    return translate_with_fallback(kKeyIndex);
  }

  // alternative interface using string keys (compile-time conversion)
  template <FixedString key_str>
  inline constexpr const char* translate() const {
    return translate<to_translation_key<key_str>()>();
  }

  // dynamic translation key
  inline const char* translate(TranslationKey key) const {
    const std::size_t key_index = static_cast<std::size_t>(key);
    DCHECK_LT(key_index, kTranslationKeyCount);
    return translate_with_fallback(key_index);
  }

  inline void translate_fmt_to(TranslationKey key,
                               char* buf,
                               const FormatArgs& args,
                               uint8_t args_count) const {
    const char* raw = translate(key);
    const std::string_view fmt = raw;
    switch (args_count) {
      case 0: vformat_to_helper<0>(buf, fmt, args); return;
      case 1: vformat_to_helper<1>(buf, fmt, args); return;
      case 2: vformat_to_helper<2>(buf, fmt, args); return;
      case 3: vformat_to_helper<3>(buf, fmt, args); return;
      case 4: vformat_to_helper<4>(buf, fmt, args); return;
      case 5: vformat_to_helper<5>(buf, fmt, args); return;
      default: return;
    }
  }

  // convenience factory functions
  static inline Translator create(LanguageId primary_lang) {
    return Translator(TranslatorConfig::create_default(primary_lang));
  }

  template <std::size_t N>
  static inline Translator create_with_fallbacks(
      LanguageId primary_lang,
      const LanguageId (&fallbacks)[N]) {
    return Translator(
        TranslatorConfig::with_fallbacks(primary_lang, fallbacks));
  }

  static inline Translator create_regional(LanguageId regional_lang) {
    return Translator(TranslatorConfig::regional(regional_lang));
  }

 private:
  // main fallback translation logic
  constexpr const char* translate_with_fallback(std::size_t key_index) const {
    // 1. try primary language
    const char* result = translate_by_lang(config_.primary_language, key_index);
    if (result && !is_string_empty(result, config_.skip_empty_strings)) {
      return result;
    }

    // 2. try fallback languages in priority order
    for (std::size_t i = 0; i < config_.fallback_count; ++i) {
      const LanguageId fallback_lang = config_.fallback_languages[i];
      result = translate_by_lang(fallback_lang, key_index);
      if (result && !is_string_empty(result, config_.skip_empty_strings)) {
        return result;
      }
    }

    // 3. final fallback to default language
    result = translate_by_lang(LanguageId::kDefault, key_index);
    if (result) {
      return result;
    }

    DCHECK(false);

    // 4. final fallback - return key name as string for debugging
    // this should never happen in properly configured translations
    return "<?>";
  }

  // check if a string is considered "empty" based on config
  static constexpr bool is_string_empty(const char* str, bool skip_empty) {
    return skip_empty && (str == nullptr || str[0] == '\0');
  }

  // get translation from specific language table
  static constexpr const char* translate_by_lang(LanguageId lang_id,
                                                 std::size_t key_index) {
    const std::size_t lang_index = static_cast<std::size_t>(lang_id);

    if (lang_index >= kTranslationTables.size()) {
      return nullptr;
    }

    if (key_index >= kTranslationKeyCount) {
      return nullptr;
    }

    const std::size_t string_index = kTranslationTables[lang_index][key_index];
    return kStringTable[string_index];
  }

  template <std::size_t N, typename... Args>
  static void vformat_to_helper(char* out_buf,
                                std::string_view fmt,
                                const FormatArgs& args,
                                Args&&... current_args) {
    if constexpr (N > 0) {
      vformat_to_helper<N - 1>(out_buf, fmt, args,
                               std::forward<Args>(current_args)...,
                               args[sizeof...(Args)]);
    } else {
      std::vformat_to(
          out_buf, fmt,
          std::make_format_args(std::forward<Args>(current_args)...));
    }
  }

  TranslatorConfig config_;
};

}  // namespace i18n

#endif  // I18N_BASE_TRANSLATOR_H_
