// The original's per-glyph text blocks and font loader blocks on Anvil's block
// replacements (text_blocks.cpp), so that the game draws, measures and wraps
// UTF-8 text, with the glyphs above U+00FF a font holds.
#pragma once

#include <anvil/anvil.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace unicode_patch {

// Replaces the blocks; returns how many.
std::size_t load_text_blocks(anvil::Host& host, std::vector<anvil::Change>& changes);

// The map label blocks; returns how many.
std::size_t load_map_blocks(anvil::Host& host, std::vector<anvil::Change>& changes);

// The font loader keeps glyphs above U+00FF (limit, 3 blocks, the font
// destructor hook); returns how many changes.
std::size_t load_font_loader(anvil::Host& host, std::vector<anvil::Change>& changes);
// How many glyphs above U+00FF the loaded fonts hold.
std::string font_loader_summary();

}  // namespace unicode_patch
