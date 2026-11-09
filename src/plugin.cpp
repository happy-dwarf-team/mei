// Mei: a port of EU4 Unicode Patch by VulonLok (MIT) to Anvil, one
// feature at a time (README.md). The text logic is the original's, built from
// the upstream/ submodule; this file only puts it on Anvil's changes instead of
// MinHook, MASM stubs and page protection.
//
// So far: the localisation import that keeps the game's text in UTF-8, the
// script and save lexer, the per-glyph text and map blocks that draw and wrap it,
// the font loader with Mei's bundled fonts, Chinese and pinyin search, UTF-8
// text input and the Steam Rich Presence text. Every symbol of
// the original is declared and verified in symbols.toml, so later features only
// add changes here.
#include <anvil/anvil.hpp>

#include <windows.h>
#include <intrin.h>

#include <array>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <string_view>
#include <vector>

#include "settings.gen.hpp"
#include "symbols.gen.hpp"

#include "fonts.hpp"
#include "game_text.hpp"
#include "localisation.hpp"
#include "native_search.hpp"
#include "native_color_strip.hpp"
#include "native_steam_presence.hpp"
#include "script_lexer.hpp"
#include "text_blocks.hpp"
#include "text_input.hpp"
#include "search_resources.hpp"
#include "unicode_search.hpp"
#include "unicode_text.hpp"

#pragma intrinsic(_ReturnAddress)

namespace {

std::uintptr_t g_image = 0;
anvil::HookOf<sym::find_country_name> g_find_country;
anvil::HookOf<sym::find_province_distance> g_province_distance;
anvil::HookOf<sym::import_legacy_localization> g_import_localisation;
anvil::HookOf<sym::parse_utf8_yaml> g_parse_yaml;
anvil::HookOf<sym::construct_colorless_text> g_colorless;

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

// The Steam Rich Presence text (a country's name, say) goes through the game's
// Windows-1252 to UTF-8 conversion before it reaches Steam, so a name that is
// already UTF-8 reached friends as Latin-1 letters. The call is redirected, so
// only that caller sees the original's construct_steam_presence: UTF-8 as it is,
// anything else through the game's conversion.
anvil::FunctionHook<void*(void* target, const void* source)> g_presence;
sym::Types::assign_editor_prefix::Pointer g_assign_text = nullptr;

eu4unicode::EngineString* native_presence(eu4unicode::EngineString* target, const eu4unicode::EngineString* source) {
    return static_cast<eu4unicode::EngineString*>(g_presence.next(target, source));
}
eu4unicode::EngineString* native_assign(eu4unicode::EngineString* target, const char* text, std::uint64_t size) {
    return static_cast<eu4unicode::EngineString*>(g_assign_text(target, text, size));
}
// The game's CString::RemoveTextColoring drops a 0xA7 byte and the byte after it, which in
// UTF-8 is the end of a character such as 大 and the start of the next one, and leaves the
// 0xC2 of a UTF-8 colour marker. The original's replacement strips whole colour markers
// (mercenary company names, institution tooltips, the game set-up's names, /save).
void* on_colorless_text(const void* source, void* target) noexcept {
    return eu4unicode::construct_colorless_text(static_cast<const eu4unicode::EngineString*>(source),
                                                static_cast<eu4unicode::EngineString*>(target));
}
void colour_warning(const char* message) { anvil::Host{}.warn("{}", message); }

// Save names: the game turns a save's name into Latin letters (CString::RemoveSpecialCharacters),
// so a Chinese country's default save became aaa1444_11_11.eu4. As in the original, the calls
// of the save menus, the ironman dialog and the save path skip it; the path keeps it unless it
// is a save's path in UTF-8, and the file system's own check of illegal characters stays.
anvil::FunctionHook<void(void* text)> g_save_path;
void on_save_path(void* text) noexcept {
    const auto* path = static_cast<const eu4unicode::EngineString*>(text);
    const std::string_view value(path->data(), static_cast<std::size_t>(path->size));
    if (value.starts_with("save games/") && eu4unicode::valid_utf8(value)) return;
    g_save_path.next(text);
}

void* on_presence_conversion(void* target, const void* source) noexcept {
    auto* text = static_cast<eu4unicode::EngineString*>(target);
    const auto* from = static_cast<const eu4unicode::EngineString*>(source);
    try {
        // The original picks this call by its return address; the redirect is that call alone.
        return eu4unicode::construct_steam_presence(text, from, eu4unicode::steam_presence_conversion_return,
                                                    &native_presence, &native_assign);
    } catch (...) {
        anvil::Host{}.warn("Rich Presence text not kept in UTF-8; the game converts it");
        return g_presence.next(target, source);
    }
}

std::uintptr_t caller_rva(void* return_address) {
    return unicode_patch::steam_caller(reinterpret_cast<std::uintptr_t>(return_address) - g_image);
}

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
    std::array<anvil::Change, 7> save_name_calls;

    void load(anvil::Host& host) override {
        g_image = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
        record_callers(host);
        if (cfg::utf8_localization.get()) {
            load_localisation(host);
            if (cfg::utf8_scripts.get()) host.info("script lexer: {} hooks", unicode_patch::load_script_lexer(host));
            if (cfg::utf8_save_names.get()) load_save_names(host);
        }
        if (cfg::search.get()) load_search(host);
        if (cfg::utf8_input.get())
            host.info("UTF-8 text input: {} changes", unicode_patch::load_text_input(host, text_blocks));
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
        const auto blocks = unicode_patch::load_text_blocks(host, text_blocks) +
                            unicode_patch::load_map_blocks(host, text_blocks);
        // Fonts keep their glyphs above U+00FF, which the text blocks draw.
        auto loader = unicode_patch::load_font_loader(host, text_blocks);
        // Font textures above 16 MiB load, as in the original: Mei's atlases and an
        // EU4DLL translation pack's fonts are larger.
        text_blocks.push_back(host.patch(sym::texture_file_limit, {0x00, 0x00, 0x00, 0x04}));
        ++loader;
        if (cfg::bundled_fonts.get()) loader += unicode_patch::load_font_redirect(host);
        host.subscribe("anvil-game.ready", [] {
            anvil::Host{}.info("{}", unicode_patch::font_loader_summary());
            if (cfg::bundled_fonts.get()) anvil::Host{}.info("{}", unicode_patch::font_redirect_summary());
        });
        host.info("localisation import: 2 hooks; text drawing: {} blocks; font loader: {} changes", blocks, loader);
        load_rich_presence(host);
        eu4unicode::native_color_assign =
            reinterpret_cast<eu4unicode::ColorStringAssign>(host.function(sym::assign_editor_prefix));
        eu4unicode::native_color_log = &colour_warning;
        host.hook(g_colorless, sym::construct_colorless_text, on_colorless_text);
        host.info("colour stripping by character: 1 hook");
    }

    void load_save_names(anvil::Host& host) {
        std::size_t i = 0;
        const auto nop = [&](const auto&... sites) {
            ((save_name_calls[i++] = host.patch(sites, {0x90, 0x90, 0x90, 0x90, 0x90})), ...);
        };
        // The in-game save and load menus, the ironman dialog and the front end's selection.
        nop(sym::save_transliterate_call_1174a99, sym::save_transliterate_call_1174e95,
            sym::save_transliterate_call_117519e, sym::save_transliterate_call_1175c2c,
            sym::save_transliterate_call_13b8de1, sym::save_transliterate_call_13b8f6c,
            sym::save_transliterate_call_13b9567);
        host.redirect(g_save_path, sym::save_path_transliterate_call_5ca246, on_save_path);
        host.info("UTF-8 save names: {} calls and 1 redirect", i);
    }

    // The UTF-8 text reaches Steam's Rich Presence as it is. Only the Steam build has
    // the call (symbols.toml declares it absent on the GOG builds).
    void load_rich_presence(anvil::Host& host) {
        try {
            g_assign_text = host.function(sym::assign_editor_prefix);
            host.redirect(g_presence, sym::presence_conversion_call_a901fe, on_presence_conversion);
            host.info("Steam Rich Presence in UTF-8: 1 call");
        } catch (const anvil::Error& e) {
            if (e.result() != ANVIL_ERROR_NOT_DECLARED) throw;
            host.info("Steam Rich Presence: no such call on this build");
        }
    }

    // Where the calls the original recognises return in this build: their call
    // instructions (symbols.toml), mapped to the Steam return addresses it checks.
    void record_callers(anvil::Host& host) {
        const auto record = [&](const auto& call, std::uintptr_t size, std::uintptr_t steam_return) {
            unicode_patch::record_caller(reinterpret_cast<std::uintptr_t>(host.address(call)) - g_image + size, steam_return);
        };
        record(sym::country_find_call_efc38f, 5, 0xEFC394);
        record(sym::country_find_call_1141fe6, 5, 0x1141FEB);
        record(sym::country_find_call_11420dd, 5, 0x11420E2);
        record(sym::province_distance_call_114218d, 5, 0x1142192);
        record(sym::province_distance_call_11421ad, 5, 0x11421B2);
        record(sym::queued_character_call_14e9cb3, 3, 0x14E9CB6);
        record(sym::commit_insert_call_153560f, 6, 0x1535615);
        record(sym::write_filter_call_1536c32, 5, 0x1536C37);
        record(sym::font_filter_call_15a05a8, 5, 0x15A05AD);
        record(sym::tooltip_insert_call_1415f33, 5, 0x1415F38);
        record(sym::lobby_chat_insert_call_83eadb, 5, 0x83EAE0);
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
