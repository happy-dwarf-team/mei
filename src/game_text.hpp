// The game's own text as the original's search code expects it. The original
// converts the game's localisation to UTF-8 when it loads (its localisation
// import), so its search code rejects any other text: a name that is not valid
// UTF-8 throws. This port does not carry that import yet, so the game's names
// and the typed query are still Windows-1252 (Östergötland is "\xD6sterg\xF6tland")
// and are converted here before they reach the original's code.
#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "native_search.hpp"

namespace unicode_patch {

// UTF-8 text as it is; anything else decoded as Windows-1252, the game's own
// encoding without the original's localisation import.
std::string utf8_game_text(std::string_view text);

// find_display_name and province_search_distance of the original on the game's
// text converted by utf8_game_text. Callers the original does not change get
// `original` with the game's strings untouched, as in the original.
std::uint64_t find_game_name(std::uintptr_t caller, const char* name, std::uint64_t length, std::uint64_t start,
                             const char* query, std::uint64_t query_length, eu4unicode::NativeFindText original);
std::int64_t province_game_distance(std::uintptr_t caller, const eu4unicode::EngineString* name,
                                    const eu4unicode::EngineString* query,
                                    eu4unicode::NativeSearchDistance original);

// The original recognises some of the game's calls by where they return in the
// Steam build (native_search.cpp, and its plugin.cpp for text input). Their call
// instructions are declared in symbols.toml for every build: record_caller maps
// where one returns in the running build to where it returns on Steam, and
// steam_caller gives that address for a return address of the running build
// (unchanged for any other caller, and on Steam).
void record_caller(std::uintptr_t rva, std::uintptr_t steam_rva) noexcept;
std::uintptr_t steam_caller(std::uintptr_t rva) noexcept;

}  // namespace unicode_patch
