// The text blocks' decoding: a UTF-8 character gives the index of its glyph in
// the game's fonts and its length; text that is not UTF-8 stays one byte per
// glyph, as the game reads it; nothing gets an index above 0xFF.
#include "text_drawing.hpp"
#include "unicode_text.hpp"

#include <cstdio>
#include <cstring>
#include <string>

namespace {

int g_failures = 0;

void check(bool ok, const char* what) {
    if (!ok) {
        std::printf("FAIL: %s\n", what);
        ++g_failures;
    }
}

std::string utf8(const char8_t* text) { return reinterpret_cast<const char*>(text); }

using unicode_patch::Glyph;

bool same(Glyph a, std::uint32_t index, std::uint32_t extra) { return a.index == index && a.extra == extra; }

// The original's decode_z (plugin.cpp) with its index, the character itself,
// replaced by the glyph index of the game's fonts.
Glyph original_decode_z(const char* text) {
    std::size_t length = 0;
    while (length < 4 && text[length]) ++length;
    const auto scalar = eu4unicode::decode({text, length});
    const auto index = scalar.valid ? unicode_patch::glyph_index(scalar.value) : static_cast<unsigned char>(*text);
    const auto consumed = scalar.bytes ? scalar.bytes : 1;
    return {index, static_cast<std::uint32_t>(consumed - 1)};
}

}  // namespace

int text_drawing_tests() {
    using namespace unicode_patch;

    // The glyph index: ASCII and Latin-1 as they are, the rest of Windows-1252 at
    // its byte, anything else '?'.
    check(glyph_index('A') == 'A' && glyph_index(0xF6) == 0xF6 && glyph_index(0xA0) == 0xA0, "Latin-1 keeps its index");
    check(glyph_index(0x2013) == 0x96 && glyph_index(0x20AC) == 0x80 && glyph_index(0x2019) == 0x92 &&
              glyph_index(0x0178) == 0x9F,
          "the rest of Windows-1252 is drawn from its byte");
    check(glyph_index(0x4E2D) == '?' && glyph_index(0x81) == '?' && glyph_index(0x20BAF) == '?',
          "characters the game's fonts lack are '?'");

    // decode_z: the localisation's UTF-8 and the game's Windows-1252 side by side.
    const auto ostra = utf8(u8"Östra Götaland");
    check(same(decode_glyph(ostra.c_str()), 0xD6, 1), "UTF-8 Ö is one glyph of two bytes");
    check(same(decode_glyph("\xD6sterg\xF6tland"), 0xD6, 0), "Windows-1252 Ö stays one byte");
    check(same(decode_glyph(utf8(u8"–").c_str()), 0x96, 2), "an en dash is the font's 0x96");
    check(same(decode_glyph(utf8(u8"中文").c_str()), '?', 2), "Chinese is '?' of three bytes");
    check(same(decode_glyph(utf8(u8"𠮷").c_str()), '?', 3), "a character outside the BMP is '?' of four bytes");
    check(same(decode_glyph("\xC3"), 0xC3, 0), "a cut sequence at the end of the text is one byte");

    // The decoder agrees with the original's (utf8::next, which throws on bad
    // input) on every 2-byte prefix and on 3- and 4-byte forms around each limit.
    int disagreements = 0;
    const unsigned char tails[] = {0x00, 0x41, 0x7F, 0x80, 0x8F, 0x90, 0x9F, 0xA0, 0xBF, 0xC0, 0xFF};
    for (int a = 1; a < 256; ++a)
        for (int b = 0; b < 256; ++b)
            for (unsigned char c : tails)
                for (unsigned char d : tails) {
                    const char text[5] = {static_cast<char>(a), static_cast<char>(b), static_cast<char>(c),
                                          static_cast<char>(d), 0};
                    const auto mine = decode_glyph(text), theirs = original_decode_z(text);
                    if (mine.index != theirs.index || mine.extra != theirs.extra) ++disagreements;
                }
    check(disagreements == 0, "the decoder agrees with the original's on every tested sequence");

    // format_scalar: only § £ ¤ advance before the game's format parser.
    check(same(format_glyph(utf8(u8"§Yred§!").c_str()), 0xA7, 1), "a UTF-8 § advances past its first byte");
    check(same(format_glyph("\xA7Yred"), 0xA7, 0), "a Windows-1252 § is one byte");
    check(same(format_glyph(utf8(u8"£gold£").c_str()), 0xA3, 1), "a UTF-8 £ advances past its first byte");
    check(same(format_glyph(ostra.c_str()), 0xD6, 0), "any other character is left to the glyph blocks");

    // decode_layout_range and format_layout_range.
    Glyph glyph{};
    check(!layout_glyph({}, glyph), "empty text ends the routine");
    check(!layout_glyph(std::string_view("\xC3", 1), glyph), "a sequence that the text's end cuts ends it");
    check(layout_glyph(std::string_view("\xC3x", 2), glyph) && same(glyph, 0xC3, 0), "a bad sequence is one byte");
    check(layout_glyph(ostra, glyph) && same(glyph, 0xD6, 1), "a character in layout");
    check(!format_layout_glyph(utf8(u8"§"), glyph), "a § at the end of the text ends the routine");
    check(!format_layout_glyph("@AB", glyph) && format_layout_glyph("@ABC", glyph) && same(glyph, '@', 0),
          "a flag token needs three letters");
    check(format_layout_glyph(utf8(u8"£gold£"), glyph) && same(glyph, 0xA3, 1), "an icon keeps its £ length");
    check(!format_layout_glyph(utf8(u8"£ gold"), glyph), "a £ that opens no icon ends the routine");
    check(format_layout_glyph(ostra, glyph) && same(glyph, 0xD6, 0), "a character is left to the measuring block");

    // native_scalar_next and the input limit.
    check(next_glyph(ostra, 0) == 2 && next_glyph(ostra, 2) == 3 && next_glyph(ostra, 99) == ostra.size(),
          "the next character's offset");
    check(next_glyph("\xD6s", 0) == 1, "Windows-1252 advances one byte");
    const std::string limit = std::string(31999, 'a') + utf8(u8"ö") + "b";
    check(bounded_text_length(limit) == 31999, "the 32000-byte limit does not cut a character");
    check(bounded_text_length(std::string(32005, 'a')) == 32000 && bounded_text_length("abc") == 3, "the limit");

    // copy_scalar: the whole character when it fits, and its glyph for the
    // measuring block after it.
    char buffer[8] = {};
    check(same(copy_glyph(ostra.data(), ostra.size(), buffer, sizeof buffer), 0xD6, 1) &&
              std::memcmp(buffer, ostra.data(), 2) == 0 && text_state().last_index == 0xD6,
          "a character is copied whole");
    char small[1] = {'x'};
    check(same(copy_glyph(ostra.data(), ostra.size(), small, 1), 0xD6, 1) && small[0] == 'x',
          "a character that does not fit is not copied");

    return g_failures;
}
