// The original's UTF-8 text input path (text_input.cpp): typed and IME-committed
// text reaches text fields as UTF-8, whole.
#pragma once

#include <anvil/anvil.hpp>

#include <cstddef>
#include <vector>

namespace unicode_patch {

// The input block and the two text field hooks; returns how many changes.
std::size_t load_text_input(anvil::Host& host, std::vector<anvil::Change>& changes);

}  // namespace unicode_patch
