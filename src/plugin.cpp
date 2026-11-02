// Unicode Patch for Anvil: a port of EU4 Unicode Patch by VulonLok (MIT), one
// feature at a time (README.md). The text logic is the original's, built from
// the upstream/ submodule; this file only puts it on Anvil's changes instead of
// MinHook, MASM stubs and page protection.
//
// So far: Chinese and pinyin search in the diplomacy country list and the
// province finder, and the localisation import that keeps the game's text in
// UTF-8, with the per-glyph text blocks that draw it. Every symbol of the original is declared and verified in
// symbols.toml, so later features only add changes here.
#include <anvil/anvil.hpp>

#include <windows.h>
#include <intrin.h>

#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <vector>

#include "settings.gen.hpp"
#include "symbols.gen.hpp"

#include "game_text.hpp"
#include "localisation.hpp"
#include "native_search.hpp"
#include "text_blocks.hpp"
#include "search_resources.hpp"
#include "unicode_search.hpp"

#pragma intrinsic(_ReturnAddress)

namespace {

std::uintptr_t g_image = 0;
anvil::HookOf<sym::find_country_name> g_find_country;
anvil::HookOf<sym::find_province_distance> g_province_distance;
anvil::HookOf<sym::import_legacy_localization> g_import_localisation;
anvil::HookOf<sym::parse_utf8_yaml> g_parse_yaml;

// The game's own functions for the original's typed calls.
std::uint64_t native_find(const char* name, std::uint64_t length, std::uint64_t start, const char* query,
                          std::uint64_t query_length) {
    return g_find_country.next(name, length, start, query, query_length);
}
std::int64_t native_distance(const eu4unicode::EngineString* name, const eu4unicode::EngineString* query) {
    return g_province_distance.next(name, query);
}

void native_parse_yaml(void* file, const void* input, const char* name, void* language, void* key, void* context) {
    g_parse_yaml.next(file, input, name, language, key, context);
}
void import_warning(const std::string& message) { anvil::Host{}.warn("{}", message); }

std::uintptr_t caller_rva(void* return_address) { return reinterpret_cast<std::uintptr_t>(return_address) - g_image; }

// The detours are reached by a jump from the game's function, so the return
// address is the game's call site, which picks the callers the original
// changes (native_search.cpp); every other caller gets the game's function.
// The game's text reaches the original's matching as UTF-8 (game_text.hpp).
std::uint64_t on_find_country(const char* name, std::uint64_t length, std::uint64_t start, const char* query,
                              std::uint64_t query_length) noexcept {
    const std::uintptr_t caller = caller_rva(_ReturnAddress());
    if (!eu4unicode::display_find_caller(caller)) return g_find_country.next(name, length, start, query, query_length);
    try {
        return unicode_patch::find_game_name(caller, name, length, start, query, query_length, &native_find);
    } catch (...) {
        anvil::Host{}.warn("country search failed; the candidate is left out");
        return UINT64_MAX;
    }
}

std::int64_t on_province_distance(const void* name, const void* query) noexcept {
    const std::uintptr_t caller = caller_rva(_ReturnAddress());
    const auto* n = static_cast<const eu4unicode::EngineString*>(name);
    const auto* q = static_cast<const eu4unicode::EngineString*>(query);
    if (!eu4unicode::province_distance_caller(caller)) return g_province_distance.next(name, query);
    try {
        return unicode_patch::province_game_distance(caller, n, q, &native_distance);
    } catch (...) {
        anvil::Host{}.warn("province search failed; the candidate is ranked last");
        return INT32_MAX / 2;
    }
}

struct UnicodePatch : anvil::Plugin {
    // The original NOPs these calls: they lower-case and strip Latin-1 accents
    // from search keys in place, which breaks UTF-8. The detours above match
    // case- and accent-insensitively themselves.
    std::array<anvil::Change, 19> search_calls;
    std::vector<anvil::Change> text_blocks;

    void load(anvil::Host& host) override {
        g_image = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
        if (cfg::utf8_localization.get()) load_localisation(host);
        if (cfg::search.get()) load_search(host);
        host.info("UTF-8 localisation {}, search {}", cfg::utf8_localization.get() ? "on" : "off",
                  cfg::search.get() ? "on" : "off");
    }

    // phase = "early": in place before the game reads its localisation.
    void load_localisation(anvil::Host& host) {
        unicode_patch::set_localisation_natives(host.function(sym::register_imported_text), &native_parse_yaml,
                                                &import_warning);
        host.hook(g_import_localisation, sym::import_legacy_localization, unicode_patch::import_localisation);
        host.hook(g_parse_yaml, sym::parse_utf8_yaml, unicode_patch::parse_localisation_file);
        // The main menu is up: the game has read its localisation.
        host.subscribe("anvil-game.ready", [] { anvil::Host{}.info("{}", unicode_patch::localisation_summary()); });
        // The game walks text byte by byte; these blocks take a UTF-8 character
        // and its glyph instead, so the imported text draws as it reads.
        const auto blocks = unicode_patch::load_text_blocks(host, text_blocks);
        host.info("localisation import: 2 hooks; text drawing: {} blocks", blocks);
    }

    void load_search(anvil::Host& host) {
        try {
            const bool custom = eu4unicode::load_search_resources(host.plugin_dir());
            host.info("{}", custom ? "custom pinyin dictionary loaded (pinyin.txt)" : "built-in pinyin dictionary");
        } catch (const std::exception& e) {
            host.warn("pinyin.txt not read, the built-in dictionary is used: {}", e.what());
        }
        eu4unicode::set_search_options({cfg::typo_tolerance.get(), cfg::fuzzy_pinyin.get()});
        host.hook(g_find_country, sym::find_country_name, on_find_country);
        host.hook(g_province_distance, sym::find_province_distance, on_province_distance);
        // Each symbol has its own type, so the sites go through a fold, not a loop.
        std::size_t i = 0;
        const auto nop = [&](const auto&... sites) {
            ((search_calls[i++] = host.patch(sites, {0x90, 0x90, 0x90, 0x90, 0x90})), ...);
        };
        nop(sym::search_lower_call_efc33b, sym::search_lower_call_f16156, sym::search_lower_call_1141475,
            sym::search_lower_call_11414be, sym::search_lower_call_114187b, sym::search_lower_call_1141b98,
            sym::search_lower_call_1141e9c, sym::search_lower_call_11434c8, sym::search_lower_call_1143a7c,
            sym::search_lower_call_1144338);
        nop(sym::search_unaccent_call_efc344, sym::search_unaccent_call_f1615e, sym::search_unaccent_call_114147e,
            sym::search_unaccent_call_11414c7, sym::search_unaccent_call_1141884, sym::search_unaccent_call_1141ba1,
            sym::search_unaccent_call_1141ea6, sym::search_unaccent_call_1143a86, sym::search_unaccent_call_1144341);
        host.info("search: 2 hooks and {} calls", i);
    }
};

}  // namespace

ANVIL_PLUGIN(UnicodePatch)
