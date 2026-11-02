#include "localisation.hpp"

#include <algorithm>
#include <atomic>
#include <format>
#include <optional>

#include "native_legacy_import.hpp"
#include "native_yaml.hpp"
#include "yaml_input.hpp"

namespace unicode_patch {

namespace {

ParseYaml g_parse_yaml = nullptr;
ImportWarning g_warn = nullptr;
std::atomic<std::uint64_t> g_values{0}, g_signed_files{0}, g_logged{0};

// The original's message (plugin.cpp, legacy_import_error), logged up to kLoggedRejections times.
void rejected(std::string_view source, int line, std::string_view key, std::size_t offset, const char* error) {
    const auto n = g_logged.fetch_add(1, std::memory_order_relaxed);
    if (!g_warn || n > kLoggedRejections) return;
    if (n == kLoggedRejections) {
        g_warn("further rejected values are only counted");
        return;
    }
    g_warn(std::format("rejected text import: {}, line {}, key {}, byte {}: {}", source.substr(0, 300), line,
                       key.substr(0, 200), offset, error));
}

}  // namespace

void set_localisation_natives(RegisterText register_text, ParseYaml parse_yaml, ImportWarning warn) {
    eu4unicode::register_imported_text = register_text;
    eu4unicode::legacy_import_diagnostic = rejected;
    g_parse_yaml = parse_yaml;
    g_warn = warn;
}

void import_localisation(const void* key, const void* value, int version, int unused, const void* context) noexcept {
    g_values.fetch_add(1, std::memory_order_relaxed);
    // noexcept itself: it reports a failure as a rejection.
    eu4unicode::import_legacy_localization(static_cast<const eu4unicode::EngineString*>(key),
                                           static_cast<const eu4unicode::EngineString*>(value), version, unused,
                                           static_cast<const eu4unicode::NativeLocalizationContext*>(context));
}

// native_yaml.cpp's parse_utf8_yaml, except that a failure to build the signed
// copy leaves the file to the game as it is instead of unwinding into it.
void parse_localisation_file(void* file, const void* input, const char* name, void* language, void* key,
                             void* context) noexcept {
    const auto* bytes = static_cast<const eu4unicode::NativeYamlBytes*>(input);
    std::optional<std::string> signed_copy;
    try {
        if (bytes->begin)
            signed_copy = eu4unicode::yaml_parse_buffer({bytes->begin, static_cast<std::size_t>(bytes->end - bytes->begin)});
    } catch (...) {
        signed_copy.reset();
    }
    if (!signed_copy) return g_parse_yaml(file, input, name, language, key, context);
    g_signed_files.fetch_add(1, std::memory_order_relaxed);
    const char* begin = signed_copy->data();
    const eu4unicode::NativeYamlBytes buffer{begin, begin + signed_copy->size(), begin + signed_copy->size()};
    g_parse_yaml(file, &buffer, name, language, key, context);
}

LocalisationCounts localisation_counts() noexcept {
    const auto legacy = eu4unicode::legacy_import_counts();
    return {g_values.load(std::memory_order_relaxed), legacy.localization, legacy.rejected,
            g_signed_files.load(std::memory_order_relaxed)};
}

std::string localisation_summary() {
    const auto c = localisation_counts();
    return std::format("localisation import: {} values kept in UTF-8, {} of them decoded from EU4DLL escapes, {} rejected; "
                       "{} files without a UTF-8 signature",
                       c.values - std::min(c.values, c.rejected), c.escaped, c.rejected, c.signed_files);
}

}  // namespace unicode_patch
