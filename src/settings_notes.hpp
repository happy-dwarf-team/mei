// Notes for anvil.log about settings that do nothing as set: four settings need
// utf8_localization, and the localisation import without the ported search drops
// accented names from the game's search (README, "In-game test of the localisation import").
#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace unicode_patch {

struct SettingState {
    bool utf8_localization;
    bool search;
    bool utf8_scripts;
    bool bundled_fonts;
    bool cjk_dates;
    bool utf8_save_names;
};

// The settings that are on but skipped because utf8_localization is off, as one log
// line; empty when there are none.
inline std::string skipped_settings_note(const SettingState& s) {
    if (s.utf8_localization) return {};
    std::vector<std::string_view> on;
    if (s.utf8_scripts) on.push_back("utf8_scripts");
    if (s.bundled_fonts) on.push_back("bundled_fonts");
    if (s.cjk_dates) on.push_back("cjk_dates");
    if (s.utf8_save_names) on.push_back("utf8_save_names");
    if (on.empty()) return {};
    std::string names;
    for (std::size_t i = 0; i < on.size(); ++i) {
        if (i > 0) names += i + 1 == on.size() ? " and " : ", ";
        names += on[i];
    }
    return names + (on.size() == 1 ? " needs" : " need") + " utf8_localization, which is off: skipped";
}

// A warning for utf8_localization without search; empty otherwise.
inline std::string search_off_note(const SettingState& s) {
    if (!s.utf8_localization || s.search) return {};
    return "utf8_localization is on and search is off: the game's own search misses accented names in UTF-8";
}

}  // namespace unicode_patch
