// Mei's bundled fonts (fonts/README.md): the game's own fonts have no glyph above
// U+00FF, so where the game would load one of them, the font loader takes Mei's atlas
// of the same size instead. A font that a mod supplies (an EU4DLL translation pack's)
// stays, as in the original (plugin.cpp load_font_atlas).
#pragma once

#include <anvil/anvil.hpp>

#include <cstddef>
#include <string>

namespace unicode_patch {

// Hooks the font loader's entry; returns how many changes.
std::size_t load_font_redirect(anvil::Host& host);
// Which fonts were replaced and which stayed.
std::string font_redirect_summary();

}  // namespace unicode_patch
