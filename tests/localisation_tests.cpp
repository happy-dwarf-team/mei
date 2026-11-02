// The localisation import as the game calls it: the values of its localisation
// files reach the game's registration in UTF-8 (the game itself would store
// Windows-1252 and '?'), old EU4DLL escapes are decoded, anything else is
// rejected, and a file without a UTF-8 signature gets one.
#include "game_text.hpp"
#include "localisation.hpp"
#include "native_legacy_import.hpp"
#include "native_yaml.hpp"

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

std::string utf8(const char8_t* text) { return reinterpret_cast<const char*>(text); }

eu4unicode::EngineString engine(const std::string& text) {
    eu4unicode::EngineString s{};
    s.size = text.size();
    s.capacity = text.size() < 16 ? 15 : text.size();
    if (text.size() < 16)
        std::memcpy(s.storage.inline_bytes, text.c_str(), text.size() + 1);
    else
        s.storage.pointer = text.c_str();
    return s;
}

struct Registration {
    void* collection;
    std::string key, value;
    int line, version;
    bool replace;
};
std::vector<Registration> g_registered;
void register_text(void* collection, const char* key, const char* value, int line, int version, bool replace) {
    g_registered.push_back({collection, key, value, line, version, replace});
}

std::vector<std::string> g_parsed;
std::vector<const void*> g_parsed_inputs;
void parse_yaml(void*, const void* input, const char*, void*, void*, void*) {
    const auto* bytes = static_cast<const eu4unicode::NativeYamlBytes*>(input);
    g_parsed.emplace_back(bytes->begin, bytes->end);
    g_parsed_inputs.push_back(input);
}

std::vector<std::string> g_warnings;
void warn(const std::string& message) { g_warnings.push_back(message); }

}  // namespace

int localisation_tests() {
    unicode_patch::set_localisation_natives(register_text, parse_yaml, warn);
    int collection = 0;
    const eu4unicode::NativeLocalizationContext context{72, true, {}, &collection};
    const auto import = [&](const std::string& key, const std::string& value, int version = 1) {
        const auto k = engine(key), v = engine(value);
        unicode_patch::import_localisation(&k, &v, version, 0, &context);
    };

    // Vanilla English: the file has UTF-8, which the game would store as "\xD6sterg\xF6tland".
    const auto ostergotland = utf8(u8"Östergötland");
    import("PROV31", ostergotland, 4);
    check(g_registered.size() == 1 && g_registered[0].key == "PROV31" && g_registered[0].value == ostergotland,
          "an accented name is stored in UTF-8");
    check(g_registered[0].collection == &collection && g_registered[0].line == 72 && g_registered[0].version == 4 &&
              g_registered[0].replace,
          "the registration gets the game's collection, line, version and replace flag");
    // What the search's conversion (587ab6e) then gets: it leaves the name as it is.
    check(unicode_patch::utf8_game_text(g_registered[0].value) == ostergotland, "the search takes the stored name as it is");

    // Text without a Windows-1252 form, which the game would store as '?'.
    const auto france = utf8(u8"法兰西");
    import("FRA", france);
    check(g_registered.size() == 2 && g_registered[1].value == france, "Chinese is stored in UTF-8");
    const auto long_value = utf8(u8"Kristianopel och Blekinge län, 𠮷");
    import("PROV_with_a_long_key_name", long_value);
    check(g_registered.size() == 3 && g_registered[2].key == "PROV_with_a_long_key_name" &&
              g_registered[2].value == long_value,
          "long keys and values and characters outside the BMP");

    // An old translation pack: EU4DLL escapes (0x10-0x13 and two Windows-1252 bytes).
    import("FRA_ADJ", utf8(u8"§Y\x10-N§! @FRA"));
    check(g_registered.size() == 4 && g_registered[3].value == utf8(u8"§Y中§! @FRA"), "EU4DLL escapes are decoded");
    check(unicode_patch::localisation_counts().escaped == 1, "decoded values are counted");

    // A file that is not UTF-8: rejected with its key and line, not stored.
    import("PROV1", "\xD6sterg\xF6tland");
    check(g_registered.size() == 4, "Windows-1252 is not stored");
    check(g_warnings.size() == 1 && g_warnings[0].find("PROV1") != std::string::npos &&
              g_warnings[0].find("line 72") != std::string::npos && g_warnings[0].find("invalid UTF-8") != std::string::npos,
          "a rejection names the key, the line and the reason");
    import("PROV2", "\x10");
    check(g_registered.size() == 4 && g_warnings.size() == 2 &&
              g_warnings[1].find("truncated legacy escape") != std::string::npos,
          "a broken escape is rejected");
    import("PROV3", "Moskva");
    check(g_registered.size() == 5 && g_registered[4].value == "Moskva", "a value after a rejection is stored");

    // Only the first rejections are logged.
    for (int i = 0; i < 30; ++i) import("BAD", "\xFF");
    check(g_warnings.size() == unicode_patch::kLoggedRejections + 1 &&
              g_warnings.back() == "further rejected values are only counted",
          "rejections are logged up to the limit, then counted");
    const auto counts = unicode_patch::localisation_counts();
    check(counts.values == 37 && counts.rejected == 32 && counts.escaped == 1, "values and rejections are counted");
    check(unicode_patch::localisation_summary().find("5 values kept in UTF-8") != std::string::npos,
          "the summary counts the stored values");

    // Localisation files: a valid UTF-8 file without the signature gets one in a copy;
    // the others reach the game's parser as they are.
    const auto parse = [&](const std::string& file) {
        const eu4unicode::NativeYamlBytes input{file.data(), file.data() + file.size(), file.data() + file.size()};
        unicode_patch::parse_localisation_file(nullptr, &input, "localisation/test_l_english.yml", nullptr, nullptr,
                                               nullptr);
        return g_parsed_inputs.back() == &input;
    };
    const auto body = utf8(u8"l_english:\n PROV31:0 \"Östergötland\"\n");
    check(!parse(body) && g_parsed.back() == "\xEF\xBB\xBF" + body, "a file without the signature gets it");
    check(parse("\xEF\xBB\xBF" + body) && g_parsed.back() == "\xEF\xBB\xBF" + body, "a signed file is left as it is");
    check(parse("l_english:\n PROV1:0 \"\xD6sterg\xF6tland\"\n"), "a file that is not UTF-8 is left to the game");
    const eu4unicode::NativeYamlBytes empty{nullptr, nullptr, nullptr};
    unicode_patch::parse_localisation_file(nullptr, &empty, "empty.yml", nullptr, nullptr, nullptr);
    check(g_parsed_inputs.back() == &empty, "an empty file is left to the game");
    check(unicode_patch::localisation_counts().signed_files == 1, "signed files are counted");

    return g_failures;
}
