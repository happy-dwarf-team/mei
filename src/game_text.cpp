#include "game_text.hpp"

#include <array>
#include <cstring>
#include <utility>

#include "unicode_text.hpp"

namespace unicode_patch {

namespace {

// 0x80-0x9F of Windows-1252; its five unassigned bytes stay C1 controls, as in
// the original's legacy_text.cpp.
constexpr std::array<std::uint32_t, 32> kCp1252High = {
    0x20AC, 0x0081, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160,
    0x2039, 0x0152, 0x008D, 0x017D, 0x008F, 0x0090, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022,
    0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0x009D, 0x017E, 0x0178};

// A string with the game's layout that points at `text`, which must outlive it.
eu4unicode::EngineString borrowed_string(const std::string& text) {
    eu4unicode::EngineString result{};
    result.size = text.size();
    if (text.size() < 16) {
        result.capacity = 15;
        std::memcpy(result.storage.inline_bytes, text.c_str(), text.size() + 1);
    } else {
        result.capacity = text.size();
        result.storage.pointer = text.c_str();
    }
    return result;
}

std::string_view view(const eu4unicode::EngineString* text) {
    return {text->data(), static_cast<std::size_t>(text->size)};
}

}  // namespace

std::string utf8_game_text(std::string_view text) {
    if (eu4unicode::valid_utf8(text)) return std::string(text);
    std::string result;
    result.reserve(text.size() * 2);
    for (const char c : text) {
        const auto byte = static_cast<unsigned char>(c);
        if (byte < 0x80)
            result += c;
        else
            result += eu4unicode::encode(byte < 0xA0 ? kCp1252High[byte - 0x80] : byte);
    }
    return result;
}

std::uint64_t find_game_name(std::uintptr_t caller, const char* name, std::uint64_t length, std::uint64_t start,
                             const char* query, std::uint64_t query_length, eu4unicode::NativeFindText original) {
    if (!eu4unicode::display_find_caller(caller)) return original(name, length, start, query, query_length);
    if (start > length) return UINT64_MAX;
    // `start` counts the game's bytes, so the name is cut before it is converted.
    const auto n = utf8_game_text(std::string_view(name + start, static_cast<std::size_t>(length - start)));
    const auto q = utf8_game_text(std::string_view(query, static_cast<std::size_t>(query_length)));
    return eu4unicode::find_display_name(caller, n.data(), n.size(), 0, q.data(), q.size(), original);
}

std::int64_t province_game_distance(std::uintptr_t caller, const eu4unicode::EngineString* name,
                                    const eu4unicode::EngineString* query,
                                    eu4unicode::NativeSearchDistance original) {
    if (!eu4unicode::province_distance_caller(caller)) return original(name, query);
    const auto n = utf8_game_text(view(name));
    const auto q = utf8_game_text(view(query));
    if (n == view(name) && q == view(query)) return eu4unicode::province_search_distance(caller, name, query, original);
    const auto converted_name = borrowed_string(n);
    const auto converted_query = borrowed_string(q);
    return eu4unicode::province_search_distance(caller, &converted_name, &converted_query, original);
}

namespace {
// Written once while the plugin loads, before any of its hooks are enabled.
std::array<std::pair<std::uintptr_t, std::uintptr_t>, 12> g_callers{};
std::size_t g_caller_count = 0;
}  // namespace

void record_caller(std::uintptr_t rva, std::uintptr_t steam_rva) noexcept {
    if (rva != steam_rva && g_caller_count < g_callers.size()) g_callers[g_caller_count++] = {rva, steam_rva};
}

std::uintptr_t steam_caller(std::uintptr_t rva) noexcept {
    for (std::size_t i = 0; i < g_caller_count; ++i) {
        if (g_callers[i].first == rva) return g_callers[i].second;
    }
    return rva;
}

}  // namespace unicode_patch
