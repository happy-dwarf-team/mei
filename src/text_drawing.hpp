// The decoding of the original's per-glyph text blocks (plugin.cpp: decode_z,
// copy_scalar, format_scalar, decode_layout_range, format_layout_range,
// construct_scalar, next_layout_scalar), for the game's own fonts. The game
// walks its text byte by byte and takes a byte as the index into the font's 256
// glyphs; the blocks (text_blocks.cpp) take one UTF-8 character instead and the
// index of its glyph, so localisation in UTF-8 draws as it does in
// Windows-1252.
//
// A character above U+00FF is drawn from the font's glyph for it when the font
// has one (the original's glyph registry, filled by the font loader blocks);
// otherwise from the glyph at its Windows-1252 byte (U+2013 is 0x96, as the
// game's fonts hold it), or '?' for characters Windows-1252 does not have,
// which the game itself shows for them. The index the game's code sees after a
// block (kerning, line breaks) is always that byte, never above 0xFF: the
// original gives such a character its own index and ports the kerning, line
// break and atlas blocks that handle it, which are not ported here.
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

namespace eu4unicode {
class FormattedText;
}

namespace unicode_patch {

// The index of a character's glyph in the game's fonts.
std::uint32_t glyph_index(std::uint32_t scalar) noexcept;

// One character of text: the index of its glyph in the font's 256-entry table,
// its bytes after the first, and the character (the byte itself for text that
// is not UTF-8), whose glyph above U+00FF the font may hold apart.
struct Glyph {
    std::uint32_t index;
    std::uint32_t extra;
    std::uint32_t scalar = 0;
    std::uint64_t packed() const noexcept { return index | static_cast<std::uint64_t>(extra) << 32; }
};

// decode_z: the character at `text`, a NUL-terminated string. Text that is not
// UTF-8 (the game's Windows-1252 literals, script and save names) is one byte.
Glyph decode_glyph(const char* text) noexcept;
// format_scalar: as decode_glyph, but only the game's format introducers (§ £ ¤)
// advance before its parser; any other character keeps extra = 0.
Glyph format_glyph(const char* text) noexcept;
// decode_layout_range: the character at the start of `text`; nothing (false)
// for empty text or a UTF-8 sequence that the end of the text cuts.
bool layout_glyph(std::string_view text, Glyph& glyph) noexcept;
// format_layout_range: as layout_glyph, with the format rules of the game's
// width routine; false also where that routine ends on a format token.
bool format_layout_glyph(std::string_view text, Glyph& glyph) noexcept;
// native_scalar_next: the offset after the character at `offset`.
std::size_t next_glyph(std::string_view text, std::size_t offset) noexcept;
// bounded_text_length: `length` cut to 32000 bytes without cutting a character.
std::size_t bounded_text_length(std::string_view text) noexcept;

// The map label routines (the original's map_scalar_size, map_scalar_count and
// map_last_scalar_offset): the bytes of the character at `offset` (1 past the
// end), the number of characters, and where the last character starts.
std::size_t glyph_size(std::string_view text, std::size_t offset) noexcept;
std::size_t glyph_count(std::string_view text) noexcept;
std::size_t last_glyph_offset(std::string_view text) noexcept;

// The text blocks' state between blocks of one routine (the original's
// last_slot, last_scalar_bytes, button_slot, button_extra).
struct TextState {
    std::uint32_t last_index = 0;
    std::uint32_t last_scalar = 0;
    std::uint32_t last_bytes = 1;
    std::uint32_t button_extra = 0;
    std::uint32_t button_scalar = 0;
};
TextState& text_state() noexcept;

// copy_scalar: copies the character at `source` to `destination` when it fits
// and remembers its glyph for the measuring block after it.
Glyph copy_glyph(const char* source, std::size_t remaining, char* destination, std::size_t available) noexcept;

// formatted_boundaries: where Unicode's line breaking rules (UAX #14, the original's
// formatted_text.cpp) allow a line to break in `text`, cached by text; nothing for
// text without a character of three or more bytes, which wraps at spaces as the
// game wraps it. The game breaks a line only at a glyph whose width field is 0 (a
// space), so Chinese and Japanese, which have no spaces, would not wrap at all.
std::shared_ptr<const eu4unicode::FormattedText> line_breaks(std::string_view text) noexcept;
// FormattedText::line_before for `breaks` that may be empty.
bool line_before(const eu4unicode::FormattedText* breaks, std::size_t offset) noexcept;

}  // namespace unicode_patch
