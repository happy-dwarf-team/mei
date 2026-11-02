#include "fonts.hpp"

#include <windows.h>

#include <array>
#include <atomic>
#include <filesystem>
#include <format>
#include <string_view>
#include <utility>

#include "symbols.gen.hpp"

#include "engine_string.hpp"

namespace unicode_patch {
namespace {

// The game's fonts and the size of Mei's atlas for each, as the original maps them
// (upstream src/font_assets.cpp player_font_path).
constexpr std::array<std::pair<std::string_view, std::string_view>, 24> kFonts{{
    {"Arial12", "ui-14"},  {"Arial12_bold", "ui-14"},     {"garamond_12", "ui-14"},   {"garamond_14", "ui-14"},
    {"garamond_14_bold", "ui-14"}, {"garamond_16", "ui-16"}, {"garamond_16_bold", "ui-16"}, {"garamond_24", "ui-24"},
    {"Mapfont", "map"},    {"standard", "ui-14"},         {"standard_18", "ui-14"},   {"standard_22", "ui-14"},
    {"tahoma_20_bold", "ui-14"}, {"tahoma_60", "map"},    {"vic_18", "ui-16"},        {"vic_18_grey", "ui-16"},
    {"vic_18s", "ui-16"},  {"vic_22", "ui-18"},           {"vic_22_bl", "ui-18"},     {"vic_22s", "ui-18"},
    {"vic_29", "ui-24"},   {"vic_29s", "ui-24"},          {"vic_36", "ui-24"},        {"vic_36s", "ui-24"},
}};
constexpr std::string_view kPrefix = "gfx/fonts/";
constexpr std::string_view kMei = "gfx/fonts/mei/";

anvil::HookOf<sym::load_font_atlas> g_load_font;
sym::Types::resolve_resource::Pointer g_resolve = nullptr;
sym::Types::assign_editor_prefix::Pointer g_assign = nullptr;
std::filesystem::path g_game_dir;

std::atomic<std::uint32_t> g_replaced{0}, g_kept{0}, g_unavailable{0};

std::filesystem::path utf8_path(std::string_view text) {
    return std::filesystem::path(std::u8string(text.begin(), text.end()));
}

// Whether the game takes a font from its own folder: the first mounted source of its
// .fnt (the game's resolver, as the original's native_font_source uses it). A mod's
// font stays even without CJK glyphs: Korean packs that need no DLL draw Hangul from
// pairs of Latin-1 glyphs of their own fonts.
bool game_font(std::string_view path) {
    const std::string fnt = std::string(path) + ".fnt";
    const auto* mount = static_cast<const std::byte*>(g_resolve(fnt.c_str()));
    if (!mount) return false;
    const auto root = *reinterpret_cast<const char* const*>(mount + 8);
    const auto prefix = *reinterpret_cast<const char* const*>(mount + 16);
    if (!root || (prefix && *prefix)) return false;
    std::error_code error;
    return std::filesystem::equivalent(utf8_path(root), g_game_dir, error) && !error;
}

void redirect(eu4unicode::EngineString* path) {
    const std::string_view requested(path->data(), static_cast<std::size_t>(path->size));
    if (!requested.starts_with(kPrefix)) return;
    const auto name = requested.substr(kPrefix.size());
    std::string_view atlas;
    for (const auto& [font, size] : kFonts)
        if (name == font) atlas = size;
    if (atlas.empty()) return;
    if (!game_font(requested)) {
        ++g_kept;
        return;
    }
    const std::string replacement = std::string(kMei) + std::string(atlas);
    if (!g_resolve((replacement + ".fnt").c_str())) {
        // The atlases come with Mei's mod; a plugin installed without it has none.
        if (g_unavailable++ == 0) anvil::Host{}.warn("Mei's fonts are not in the game's files ({}); is Mei's mod enabled?", kMei);
        return;
    }
    g_assign(path, replacement.data(), replacement.size());
    ++g_replaced;
}

void on_load_font(void* font) noexcept {
    // The font's path and its path for the large interface (the original's offsets).
    for (const auto offset : {0xE0, 0x100}) {
        try {
            redirect(reinterpret_cast<eu4unicode::EngineString*>(static_cast<std::byte*>(font) + offset));
        } catch (const std::exception& e) {
            anvil::Host{}.warn("font redirect failed: {}", e.what());
        }
    }
    g_load_font.next(font);
}

}  // namespace

std::size_t load_font_redirect(anvil::Host& host) {
    wchar_t exe[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    g_game_dir = std::filesystem::path(exe).parent_path();
    g_resolve = host.function(sym::resolve_resource);
    g_assign = host.function(sym::assign_editor_prefix);
    host.hook(g_load_font, sym::load_font_atlas, on_load_font);
    return 1;
}

std::string font_redirect_summary() {
    return std::format("Mei's fonts: {} font paths replaced, {} of mods kept{}", g_replaced.load(), g_kept.load(),
                       g_unavailable ? std::format(", {} not replaced (no atlases)", g_unavailable.load()) : "");
}

}  // namespace unicode_patch
