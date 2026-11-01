// What the port relies on when it removes the game's own lower-casing and
// accent stripping from the search paths (the 19 search_*_call patches): the
// matching in the original's search code must still find vanilla names the way
// the game did, and the Chinese and pinyin forms of its README.
#include "unicode_search.hpp"

#include <cstdio>
#include <string>

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

    std::printf("%s\n", g_failures == 0 ? "ok" : "failed");
    return g_failures == 0 ? 0 : 1;
}
