#include "text_drawing.hpp"

#include <cstring>

#include "formatted_text.hpp"

namespace unicode_patch {

namespace {

// Windows-1252 0x80-0x9F; 0 where it has no character.
constexpr std::uint16_t kWindows1252High[32] = {
    0x20AC, 0,      0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0,      0x017D, 0,
    0,      0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0,      0x017E, 0x0178};

struct Decoded {
    std::uint32_t value;
    std::uint32_t bytes;
    bool valid;
};

// eu4unicode::decode without its exception: the blocks run for every glyph of
// every frame, and text that is not UTF-8 is common here (see the header).
// Accepts what utf8::next accepts (no overlong forms, surrogates or values
// above U+10FFFF); anything else is one invalid byte.
Decoded decode_utf8(std::string_view text) noexcept {
    if (text.empty()) return {0, 0, false};
    const auto at = [&](std::size_t i) { return static_cast<unsigned char>(text[i]); };
    const unsigned char lead = at(0);
    if (lead < 0x80) return {lead, 1, true};
    std::uint32_t length = 0, value = 0;
    unsigned char low = 0x80, high = 0xBF;
    if (lead >= 0xC2 && lead <= 0xDF) {
        length = 2;
        value = lead & 0x1F;
    } else if (lead >= 0xE0 && lead <= 0xEF) {
        length = 3;
        value = lead & 0x0F;
        if (lead == 0xE0) low = 0xA0;
        if (lead == 0xED) high = 0x9F;
    } else if (lead >= 0xF0 && lead <= 0xF4) {
        length = 4;
        value = lead & 0x07;
        if (lead == 0xF0) low = 0x90;
        if (lead == 0xF4) high = 0x8F;
    } else {
        return {0xFFFD, 1, false};
    }
    if (text.size() < length) return {0xFFFD, 1, false};
    for (std::uint32_t i = 1; i < length; ++i) {
        const unsigned char next = at(i);
        if (next < (i == 1 ? low : 0x80) || next > (i == 1 ? high : 0xBF)) return {0xFFFD, 1, false};
        value = value << 6 | (next & 0x3F);
    }
    return {value, length, true};
}

// native_scalar: an invalid byte stands for itself.
Decoded native_glyph(std::string_view text) noexcept {
    auto d = decode_utf8(text);
    if (!d.valid && !text.empty()) d.value = static_cast<unsigned char>(text.front());
    return d;
}

std::string_view first_bytes(const char* text) noexcept {
    std::size_t length = 0;
    while (length < 4 && text[length]) ++length;
    return {text, length};
}

TextState g_state;

}  // namespace

std::uint32_t glyph_index(std::uint32_t scalar) noexcept {
    if (scalar < 0x80 || (scalar >= 0xA0 && scalar <= 0xFF)) return scalar;
    for (std::uint32_t i = 0; i < 32; ++i)
        if (kWindows1252High[i] != 0 && kWindows1252High[i] == scalar) return 0x80 + i;
    return '?';
}

Glyph decode_glyph(const char* text) noexcept {
    const auto bytes = first_bytes(text);
    const auto d = decode_utf8(bytes);
    // As decode_z: text that is not UTF-8 is the glyph of its first byte.
    const std::uint32_t index = d.valid ? glyph_index(d.value) : static_cast<unsigned char>(*text);
    const std::uint32_t consumed = d.bytes ? d.bytes : 1;
    return {index, consumed - 1};
}

Glyph format_glyph(const char* text) noexcept {
    const auto glyph = decode_glyph(text);
    // Only the game's format introducers advance before its parser; any other
    // character reaches the glyph blocks, which advance past it.
    if (glyph.index == 0xA7 || glyph.index == 0xA3 || glyph.index == 0xA4) return glyph;
    return {glyph.index, 0};
}

bool layout_glyph(std::string_view text, Glyph& glyph) noexcept {
    // native_measure_scalar: a UTF-8 sequence that the end of the text cuts is
    // no character (the routine ends), not single bytes.
    if (text.empty()) return false;
    const auto lead = static_cast<unsigned char>(text.front());
    const std::size_t expected = lead >= 0xC2 && lead <= 0xDF ? 2 : lead >= 0xE0 && lead <= 0xEF ? 3 : lead >= 0xF0 && lead <= 0xF4 ? 4 : 1;
    if (text.size() < expected) {
        bool continuation = true;
        for (std::size_t i = 1; i < text.size(); ++i)
            continuation = continuation && (static_cast<unsigned char>(text[i]) & 0xC0) == 0x80;
        if (continuation) return false;
    }
    const auto d = native_glyph(text);
    glyph = {d.valid ? glyph_index(d.value) : d.value, d.bytes - 1};
    return true;
}

bool format_layout_glyph(std::string_view text, Glyph& glyph) noexcept {
    if (!layout_glyph(text, glyph)) return false;
    const auto value = native_glyph(text).value;
    const auto length = text.size();
    const std::uint32_t bytes = glyph.extra + 1;
    if (value == 0xA7 && length <= bytes) return false;
    if (value == 0xA3) {
        try {
            if (eu4unicode::native_text_unit(text, 0).kind != eu4unicode::TextUnitKind::icon) return false;
        } catch (...) {
            return false;
        }
    }
    // These fixed-size ASCII tokens are specific to this native width routine.
    if ((value == '@' && length < 4) || (value == '{' && length < 3)) return false;
    if (value != 0xA7 && value != 0xA3 && value != 0xA4) glyph.extra = 0;
    return true;
}

std::size_t next_glyph(std::string_view text, std::size_t offset) noexcept {
    if (offset >= text.size()) return text.size();
    return offset + native_glyph(text.substr(offset)).bytes;
}

std::size_t bounded_text_length(std::string_view text) noexcept {
    constexpr std::size_t limit = 32000;
    if (text.size() <= limit) return text.size();
    std::size_t end = limit;
    // A character occupies at most four bytes: find its possible first byte and
    // keep it only if the whole character fits.
    while (end && limit - end < 3 && (static_cast<unsigned char>(text[end]) & 0xC0) == 0x80) --end;
    const auto d = decode_utf8(text.substr(end));
    return d.valid && end + d.bytes > limit ? end : limit;
}

TextState& text_state() noexcept { return g_state; }

Glyph copy_glyph(const char* source, std::size_t remaining, char* destination, std::size_t available) noexcept {
    const auto d = decode_utf8({source, remaining});
    const std::uint32_t consumed = d.bytes ? d.bytes : 1;
    g_state.last_index = d.valid ? glyph_index(d.value) : static_cast<unsigned char>(*source);
    if (consumed <= available) std::memcpy(destination, source, consumed);
    return {g_state.last_index, consumed - 1};
}

}  // namespace unicode_patch
