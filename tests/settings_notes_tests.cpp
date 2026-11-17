// The log notes about settings that do nothing as set.
#include "settings_notes.hpp"

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

int settings_notes_tests() {
    using unicode_patch::search_off_note;
    using unicode_patch::SettingState;
    using unicode_patch::skipped_settings_note;

    const SettingState defaults{true, true, true, true, true, true};
    check(skipped_settings_note(defaults).empty(), "the defaults skip nothing");
    check(search_off_note(defaults).empty(), "the defaults need no warning");

    SettingState off = defaults;
    off.utf8_localization = false;
    check(skipped_settings_note(off) ==
              "utf8_scripts, bundled_fonts, cjk_dates and utf8_save_names need utf8_localization, which is off: skipped",
          "all four skipped");
    check(search_off_note(off).empty(), "search off matters only with the import");

    SettingState one{false, true, false, true, false, false};
    check(skipped_settings_note(one) == "bundled_fonts needs utf8_localization, which is off: skipped", "one skipped");

    SettingState two{false, true, true, false, false, true};
    check(skipped_settings_note(two) == "utf8_scripts and utf8_save_names need utf8_localization, which is off: skipped",
          "two skipped");

    check(skipped_settings_note({false, false, false, false, false, false}).empty(), "nothing on, nothing skipped");

    SettingState no_search = defaults;
    no_search.search = false;
    check(!search_off_note(no_search).empty(), "the import without the search warns");
    check(skipped_settings_note(no_search).empty(), "and skips nothing");
    return g_failures;
}
