// The original's localisation import on Anvil's hooks. The game reads its
// localisation files as UTF-8 and converts every value to Windows-1252 before
// it stores it (0x16FD650 through 0x17059E0; a character without a Windows-1252
// form becomes '?'). The original replaces that function: valid UTF-8 is stored
// as it is, EU4DLL escapes of old translation packs are decoded to UTF-8 and
// anything else is rejected (native_legacy_import.cpp). It also gives a
// localisation file without a UTF-8 signature the one the game's parser needs
// (native_yaml.cpp).
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace unicode_patch {

// The game's functions, with the signatures symbols.toml declares.
using RegisterText = void (*)(void* collection, const char* key, const char* value, int line, int version,
                              bool replace);
using ParseYaml = void (*)(void* file, const void* input, const char* name, void* language, void* key, void* context);
// Gets one line about a rejected value.
using ImportWarning = void (*)(const std::string& message);

// Before the hooks are made.
void set_localisation_natives(RegisterText register_text, ParseYaml parse_yaml, ImportWarning warn);

// The detours of import_legacy_localization and parse_utf8_yaml.
void import_localisation(const void* key, const void* value, int version, int unused, const void* context) noexcept;
void parse_localisation_file(void* file, const void* input, const char* name, void* language, void* key,
                             void* context) noexcept;

// Rejections are logged up to this many; later ones are only counted.
inline constexpr std::uint64_t kLoggedRejections = 20;

struct LocalisationCounts {
    std::uint64_t values;     // values the game handed to the import
    std::uint64_t escaped;    // of them, with EU4DLL escapes, decoded
    std::uint64_t rejected;   // of them, not stored (invalid UTF-8 or a bad escape)
    std::uint64_t signed_files;  // files without a UTF-8 signature that got one
};
LocalisationCounts localisation_counts() noexcept;
std::string localisation_summary();

// The lexer's token buffer holds an EU4DLL escape marker. The game reads millions
// of tokens and almost none has one, so this comes before the original's import.
bool token_has_legacy_escape(const char* text, std::size_t capacity) noexcept;

struct LexerCounts {
    std::uint64_t script;    // script tokens decoded from EU4DLL escapes
    std::uint64_t binary;    // binary save tokens decoded from them
    std::uint64_t rejected;  // localisation values and tokens rejected, as in LocalisationCounts
};
LexerCounts lexer_counts() noexcept;
std::string lexer_summary();

}  // namespace unicode_patch
