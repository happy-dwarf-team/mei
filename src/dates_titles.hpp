// Dates and combat titles in the translation's word order (cjk_format.hpp) on the game's
// calls: the top bar's date (as the original), the five month-and-year joins EU4DLL
// changes (operator+ in the date's end-of-month and long month-and-year texts, as the
// original, and in an heir's claim strength, the rebels' outliner entry and the game
// set-up), and every combat title (0x23A900): battles, sieges and occupations. The long date of the localisation
// (LONG_EU3_DATE_STRING) is the translation's own and needs nothing.
#pragma once

#include <cstddef>

#include <anvil/anvil.hpp>

namespace unicode_patch {

// Returns the number of changes.
std::size_t load_dates_titles(anvil::Host& host);

}  // namespace unicode_patch
