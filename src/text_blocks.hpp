// The original's per-glyph text blocks on Anvil's block replacements
// (text_blocks.cpp), so that the game draws, measures and wraps UTF-8 text.
#pragma once

#include <anvil/anvil.hpp>

#include <cstddef>
#include <vector>

namespace unicode_patch {

// Replaces the blocks; returns how many.
std::size_t load_text_blocks(anvil::Host& host, std::vector<anvil::Change>& changes);

}  // namespace unicode_patch
