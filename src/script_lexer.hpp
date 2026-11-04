// The original's script and save lexer on Anvil's hooks (native_script_bom.cpp,
// native_legacy_import.cpp). The game reads scripts and saves byte by byte, so
// UTF-8 passes through it as it is, except for a signature, which it would take
// as the start of the first token. EU4DLL translation packs keep their names in
// scripts as escapes, and old saves hold them too; the original decodes them to
// UTF-8 in each finished text token, so memory holds what the localisation import
// keeps and the text blocks draw it. Windows-1252 names stay as they are.
#pragma once

#include <cstddef>

#include <anvil/anvil.hpp>

namespace unicode_patch {

// Needs set_localisation_natives first: rejected tokens are reported as rejected
// values are. Returns the number of hooks.
std::size_t load_script_lexer(anvil::Host& host);

}  // namespace unicode_patch
