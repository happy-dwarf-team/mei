// What the port relies on when it removes the game's own lower-casing and
// accent stripping from the search paths (the 19 search_*_call patches): the
// matching in the original's search code must still find vanilla names the way
// the game did, and the Chinese and pinyin forms of its README.
#include "game_text.hpp"
#include "unicode_search.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace {

int g_failures = 0;

void check(bool ok, const char* what) {
    if (!ok) {
        std::printf("FAIL: %s\n", what);
        ++g_failures;
    }
}

}  // namespace

int main() {
    using eu4unicode::display_search_contains;
    eu4unicode::set_search_options({true, false});

    // Vanilla English: the game lower-cased both sides and stripped Latin-1 accents.
    check(display_search_contains("France", "fra"), "case-insensitive prefix");
    check(display_search_contains("France", "FRANCE"), "upper-case query");
    check(display_search_contains("Holy Roman Empire", "roman"), "substring");
    check(display_search_contains(reinterpret_cast<const char*>(u8"Österreich"), "osterreich"), "accent-insensitive");
    check(display_search_contains(reinterpret_cast<const char*>(u8"Côte d'Ivoire"), "cote"), "accent in the middle");
    check(!display_search_contains("France", "spain"), "no false match");

    // The README's example: 法兰西 by characters, full pinyin, initials and a mix.
    const std::string france = reinterpret_cast<const char*>(u8"法兰西");
    check(display_search_contains(france, france), "Chinese characters");
    check(display_search_contains(france, "falanxi"), "full pinyin");
    check(display_search_contains(france, "flx"), "initials");
    check(display_search_contains(france, reinterpret_cast<const char*>(u8"法lanxi")), "mixed input");
    check(!display_search_contains(france, "deguo"), "other pinyin does not match");

    // The game's names without the original's localisation import are Windows-1252,
    // which the original's matching rejects (it throws); the plugin converts them.
    using unicode_patch::utf8_game_text;
    const std::string ostergotland_1252 = "\xD6sterg\xF6tland";
    const std::string ostergotland = reinterpret_cast<const char*>(u8"Östergötland");
    check(utf8_game_text(ostergotland_1252) == ostergotland, "Windows-1252 to UTF-8");
    check(utf8_game_text(ostergotland) == ostergotland, "UTF-8 stays as it is");
    check(utf8_game_text("\x80\x8A\x9F") == reinterpret_cast<const char*>(u8"€ŠŸ"), "0x80-0x9F of Windows-1252");
    check(utf8_game_text("Moskva") == "Moskva", "ASCII");

    // The province finder (FUN_141141e00): a found name ranks above every distance,
    // and the distance ranks the rest; the 19 NOPs leave both strings as the game has them.
    constexpr std::uintptr_t finder_find = 0x11420e2, finder_distance = 0x1142192;
    const auto not_called_find = [](const char*, std::uint64_t, std::uint64_t, const char*, std::uint64_t) {
        return std::uint64_t{42};
    };
    const auto find = [&](const std::string& name, const std::string& query) {
        return unicode_patch::find_game_name(finder_find, name.data(), name.size(), 0, query.data(), query.size(),
                                             not_called_find);
    };
    bool threw = false;
    try {
        check(find(ostergotland_1252, "ostergotland") == 0, "finder finds a Windows-1252 name");
        check(find(ostergotland_1252, "\xF6sterg") == 0, "finder takes a Windows-1252 query");
        check(find("Moskva", "MOSKV") == 0, "finder, upper-case query");
        check(find("Mosul", "MOSKV") == UINT64_MAX, "finder, no match");
    } catch (...) {
        threw = true;
    }
    check(!threw, "finder does not throw on the game's text");
    check(unicode_patch::find_game_name(0x1234, "x", 1, 0, "y", 1, not_called_find) == 42,
          "other callers get the game's function");

    const auto engine = [](const std::string& text) {
        eu4unicode::EngineString s{};
        s.size = text.size();
        s.capacity = text.size() < 16 ? 15 : text.size();
        if (text.size() < 16)
            std::memcpy(s.storage.inline_bytes, text.c_str(), text.size() + 1);
        else
            s.storage.pointer = text.c_str();
        return s;
    };
    // The game's Levenshtein distance stands in for its own function.
    const auto levenshtein = [](const eu4unicode::EngineString* a, const eu4unicode::EngineString* b) {
        const std::string_view x(a->data(), a->size), y(b->data(), b->size);
        std::vector<std::int64_t> row(y.size() + 1);
        for (std::size_t j = 0; j <= y.size(); ++j) row[j] = static_cast<std::int64_t>(j);
        for (std::size_t i = 1; i <= x.size(); ++i) {
            std::int64_t diagonal = row[0];
            row[0] = static_cast<std::int64_t>(i);
            for (std::size_t j = 1; j <= y.size(); ++j) {
                const std::int64_t up = row[j];
                row[j] = std::min({row[j] + 1, row[j - 1] + 1, diagonal + (x[i - 1] == y[j - 1] ? 0 : 1)});
                diagonal = up;
            }
        }
        return row[y.size()];
    };
    const auto distance = [&](const std::string& name, const std::string& query) {
        const auto n = engine(name), q = engine(query);
        return unicode_patch::province_game_distance(finder_distance, &n, &q, levenshtein);
    };
    threw = false;
    try {
        check(distance(ostergotland_1252, "ostergotland") == 0, "distance, same Windows-1252 name");
        const auto latin_name = engine("ostra gotaland"), latin_query = engine("ostergotland");
        check(distance("\xD6stra G\xF6taland", "ostergotland") == levenshtein(&latin_name, &latin_query),
              "distance of a Windows-1252 name on its Latin key");
        check(distance("Mosul", "MOSKV") == 2, "distance, upper-case query");
        check(distance("Kristianopel och Blekinge l\xE4n", "ostergotland") > 2, "long Windows-1252 name");
    } catch (...) {
        threw = true;
    }
    check(!threw, "distance does not throw on the game's text");

    std::printf("%s\n", g_failures == 0 ? "ok" : "failed");
    return g_failures == 0 ? 0 : 1;
}
