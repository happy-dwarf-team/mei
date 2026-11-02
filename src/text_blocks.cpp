// The original's per-glyph text blocks (text_hooks.asm, layout_hooks.asm,
// popup_hooks.asm) as
// Anvil block replacements. Each callback does what the original's MASM stub
// does with the registers, flags and continuation, with the decoding of
// text_drawing.cpp. Offsets like [rsp+138h] in a stub address its own frame,
// 0xF0 below the game's stack pointer (SAVE_CONTEXT), so [rsp+138h] is the
// game's rsp+48h.
//
// Not ported: the blocks that only serve the original's own glyphs (shaped
// paragraphs, atlas pages, line breaks and kerning above U+00FF). With the
// game's fonts no glyph index is above 0xFF (text_drawing.hpp), so the game's
// own code at those places does what the stubs' native paths do.
#include "text_blocks.hpp"

#include <cstdint>
#include <cstring>
#include <format>

#include <windows.h>

#include "symbols.gen.hpp"

#include "engine_string.hpp"
#include "glyph_registry.hpp"
#include "text_drawing.hpp"

namespace unicode_patch {

namespace {

using anvil::Cpu;
using eu4unicode::EngineString;

constexpr std::uintptr_t kCarry = 0x1, kParity = 0x4, kAdjust = 0x10, kZero = 0x40, kSign = 0x80, kOverflow = 0x800;
constexpr std::uintptr_t kArithmetic = kCarry | kParity | kAdjust | kZero | kSign | kOverflow;

bool even_parity(std::uint8_t value) noexcept {
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return (value & 1) == 0;
}

// The flags of `test value, value` on a 64-bit register.
void test_flags(Cpu& cpu, std::uint64_t value) noexcept {
    cpu.rflags &= ~kArithmetic;
    if (value == 0) cpu.rflags |= kZero;
    if (value >> 63) cpu.rflags |= kSign;
    if (even_parity(static_cast<std::uint8_t>(value))) cpu.rflags |= kParity;
}

// The flags of `cmp a, b` on 8 or 32 bits.
template <class T> void compare_flags(Cpu& cpu, T a, T b) noexcept {
    constexpr int top = sizeof(T) * 8 - 1;
    const T result = static_cast<T>(a - b);
    cpu.rflags &= ~kArithmetic;
    if (a < b) cpu.rflags |= kCarry;
    if (result == 0) cpu.rflags |= kZero;
    if ((result >> top) & 1) cpu.rflags |= kSign;
    if ((((a ^ b) & (a ^ result)) >> top) & 1) cpu.rflags |= kOverflow;
    if ((a ^ b ^ result) & 0x10) cpu.rflags |= kAdjust;
    if (even_parity(static_cast<std::uint8_t>(result))) cpu.rflags |= kParity;
}

template <class T> T& at(std::uintptr_t address) noexcept { return *reinterpret_cast<T*>(address); }
const char* text(std::uintptr_t address) noexcept { return reinterpret_cast<const char*>(address); }
// A 32-bit register write, which clears the upper half.
std::uintptr_t low(std::uint64_t value) noexcept { return static_cast<std::uint32_t>(value); }
// movss xmm, dword ptr [address]
void load_float(AnvilXmm& xmm, std::uintptr_t address) noexcept {
    xmm = {};
    xmm.u32[0] = at<std::uint32_t>(address);
}
// The game's own glyph lookup: the font's table of 256 glyph pointers.
// The glyph of a character: the font's own record above U+00FF when the font
// loader stored one (the original's find_supplementary_glyph, without its
// paragraph and atlas glyphs), else the game's table of 256 glyph pointers.
std::uintptr_t glyph(std::uintptr_t table, const Glyph& g) noexcept {
    if (g.scalar > 0xFF)
        if (void* own = eu4unicode::find_unicode_glyph(reinterpret_cast<void* const*>(table), g.scalar))
            return reinterpret_cast<std::uintptr_t>(own);
    return at<std::uintptr_t>(table + g.index * 8);
}
// A UTF-8 '£' (C2 A3), the icon delimiter.
bool icon_tail(std::uintptr_t address) noexcept {
    return at<std::uint8_t>(address) == 0xC2 && at<std::uint8_t>(address + 1) == 0xA3;
}
std::string_view engine_text(std::uintptr_t address) noexcept {
    const auto& s = at<const EngineString>(address);
    return {s.data(), static_cast<std::size_t>(s.size)};
}

struct Natives {
    std::uint8_t* copy_buffer;
    char* (*repeat_text)(void* target, std::uint64_t count, std::uint8_t character);
    void* (*append_text)(void* target, const char* text, std::uint64_t size);
    std::uintptr_t main_measure_entry, main_plain_entry, button_plain_entry, button_draw_plain_entry,
        bitmap_plain_entry, button_loop, button_end, alternate_end, alternate_plain_entry, popup_plain_entry,
        popup_draw_plain_entry, font_allocate, font_duplicate, font_initialize, font_skip;
    void* (*engine_new)(std::uint64_t size);
} g;

// --- the main text routine (0x15988E0) ---

void main_copy(Cpu& c) noexcept {
    c.r9 = static_cast<std::uint64_t>(static_cast<std::int64_t>(static_cast<std::int32_t>(c.rdi))) + at<std::uint64_t>(c.rbp - 8);
    c.rdx = at<std::uint64_t>(c.rbp - 8);
    c.rcx = static_cast<std::uint64_t>(static_cast<std::int64_t>(static_cast<std::int32_t>(c.rsi)));
    c.r11 = reinterpret_cast<std::uintptr_t>(g.copy_buffer);
    const std::uint64_t remaining = at<std::uint64_t>(c.r12 + 0x10) - c.rdi;
    const std::uint32_t available = 0x7D00u - static_cast<std::uint32_t>(c.rsi);
    const auto glyph_copy = copy_glyph(text(c.r9), remaining, reinterpret_cast<char*>(c.r11 + c.rsi), available);
    c.rdi = low(c.rdi + glyph_copy.extra);
    c.rsi = low(c.rsi + glyph_copy.extra + 1);
    c.r9 += glyph_copy.extra;
    c.rax = glyph_copy.index;
    if (glyph_copy.index > 0xFF) c.rip = g.main_measure_entry;
}

void main_measure(Cpu& c) noexcept {
    c.rax = text_state().last_index;
    c.rcx = glyph(c.r14 + 0x120, Glyph{text_state().last_index, 0, text_state().last_scalar});
    at<std::uintptr_t>(c.rbp) = c.rcx;
    test_flags(c, c.rcx);
}

void main_format(Cpu& c) noexcept {
    c.r8 = c.rcx + c.r9;
    const auto f = format_glyph(text(c.r8));
    if (f.index > 0xFF) {
        c.rip = g.main_plain_entry;
        return;
    }
    c.r15 = low(c.r15 + f.extra);
    c.rcx += f.extra;
    c.r8 += f.extra;
    c.rax = f.index;
    compare_flags<std::uint8_t>(c, static_cast<std::uint8_t>(f.index), 0xA7);
}

void main_icon_copy(Cpu& c) noexcept {
    if (icon_tail(c.r9)) {
        at<std::uint8_t>(c.r8) = 0xA3;
        c.rdi = low(c.rdi + 1);
        c.rsi = low(c.rsi + 1);
    }
    at<std::uint8_t>(c.rbp + c.rdx + 0x1D0) = 0;
}

void main_icon_draw(Cpu& c) noexcept {
    if (icon_tail(c.r8)) c.r15 = low(c.r15 + 1);
    at<std::uint8_t>(c.rbp + c.rcx + 0x1D0) = 0;
}

void main_draw(Cpu& c) noexcept {
    const auto d = decode_glyph(text(c.rcx + c.r9));
    c.r15 = low(c.r15 + d.extra);
    c.r8 = d.index;
    load_float(c.xmm[3], c.r14 + 0x968);
    c.rdx = glyph(c.r14 + 0x120, d);
    test_flags(c, c.rdx);
}

// --- the button text routine (0x1596660) ---

void button_copy(Cpu& c) noexcept {
    const char* source = text(c.rax + c.rbx);
    const auto d = decode_glyph(source);
    text_state().button_extra = d.extra;
    const std::uint64_t size = d.extra + 1;
    char* destination = g.repeat_text(reinterpret_cast<void*>(c.rsp + 0x48), size, static_cast<std::uint8_t>(*source));
    std::memcpy(destination, source, size);
    c.rax = reinterpret_cast<std::uintptr_t>(destination);
}

void button_format(Cpu& c) noexcept {
    c.rax = c.r15 >= 0x10 ? c.r12 : c.rbp - 0x38;
    const auto f = format_glyph(text(c.rax + c.rbx));
    if (f.index > 0xFF) {
        c.rip = g.button_plain_entry;
        return;
    }
    c.r14 = low(c.r14 + f.extra);
    c.rbx += f.extra;
    if (f.extra) text_state().button_extra = 0;
    compare_flags<std::uint8_t>(c, at<std::uint8_t>(c.rbx + c.rax), 0xA7);
}

void button_icon_copy(Cpu& c) noexcept {
    std::uintptr_t source = c.rbp - 0x38;
    if (at<std::uint64_t>(c.rbp - 0x20) >= 0x10) source = at<std::uintptr_t>(source);
    source += c.r14;
    if (icon_tail(source)) {
        g.append_text(reinterpret_cast<void*>(c.rsp + 0x78), text(source + 1), 1);
        c.r14 = low(c.r14 + 1);
    }
    at<std::uint8_t>(c.rbp + c.rdx + 0xA0) = 0;
}

void button_measure(Cpu& c) noexcept {
    c.r9 = low(c.r14);
    const auto d = decode_glyph(text(c.r9 + c.rax));
    c.r9 += d.extra;
    c.rax = d.index;
    c.r11 = glyph(c.rcx, d);
    test_flags(c, c.r11);
}

void button_advance(Cpu& c) noexcept {
    c.r14 = low(c.r14 + text_state().button_extra + 1);
    const auto bound = at<std::int32_t>(c.rbp - 0x28);
    compare_flags<std::uint32_t>(c, static_cast<std::uint32_t>(c.r14), static_cast<std::uint32_t>(bound));
    if (static_cast<std::int32_t>(c.r14) < bound) {
        c.rip = g.button_loop;
        return;
    }
    c.r15 = at<std::uint32_t>(c.rbp + 0x21C8);
    c.rip = g.button_end;
}

void button_draw_format(Cpu& c) noexcept {
    const std::uintptr_t base = c.r12 >= 0x10 ? c.r13 : c.rbp - 0x60;
    c.rax = base;
    c.rcx = low(c.r15);
    const auto f = format_glyph(text(c.rcx + base));
    if (f.index > 0xFF) {
        c.rip = g.button_draw_plain_entry;
        return;
    }
    c.r15 = low(c.r15 + f.extra);
    c.rcx += f.extra;
    compare_flags<std::uint8_t>(c, at<std::uint8_t>(base + c.rcx), 0xA7);
    c.rax = c.rbp - 0x60;
}

void button_icon_draw(Cpu& c) noexcept {
    c.rax = c.r12 >= 0x10 ? c.r13 : c.rbp - 0x60;
    if (icon_tail(c.rax + c.r15)) c.r15 = low(c.r15 + 1);
    at<std::uint8_t>(c.rbp + c.rdx + 0xA0) = 0;
}

void button_draw(Cpu& c) noexcept {
    c.rcx = low(c.r15);
    const auto d = decode_glyph(text(c.rcx + c.rax));
    c.r15 = low(c.r15 + d.extra);
    c.rax = d.index;
    load_float(c.xmm[11], c.rdx + 0x848);
    c.r8 = glyph(c.rdx, d);
    at<std::uintptr_t>(c.rbp + 0x58) = c.r8;
    test_flags(c, c.r8);
}

// --- the text width routines (0x159B470, 0x159B7C0, 0x159EC30, 0x159F510) and
// the input limit of the main routine ---

void text_limit(Cpu& c) noexcept {
    c.r15 = low(bounded_text_length({text(c.rdi), static_cast<std::uint32_t>(c.r15)}));
    c.rax = 0x7D00;
}

void bitmap_format(Cpu& c) noexcept {
    c.r9 = at<std::uint64_t>(c.rbx + 0x18);
    c.rcx = c.r9 >= 0x10 ? at<std::uintptr_t>(c.rbx) : c.rbx;
    const auto f = format_glyph(text(c.rcx + c.rdi));
    if (f.index > 0xFF) {
        c.rip = g.bitmap_plain_entry;
        return;
    }
    c.rdi = low(c.rdi + f.extra);
    compare_flags<std::uint8_t>(c, at<std::uint8_t>(c.rdi + c.rcx), 0xA7);
}

void bitmap_icon_end(Cpu& c) noexcept {
    // RDX still addresses the source string at this loop exit.
    if (icon_tail(c.rdx + c.rdi)) c.rdi = low(c.rdi + 1);
    c.rax = at<std::uintptr_t>(c.r14);
    c.rdx = c.rsp + 0x20;
    at<std::uint8_t>(c.rsp + c.rcx + 0x20) = 0;
}

void bitmap_measure(Cpu& c) noexcept {
    const auto d = decode_glyph(text(c.rdi + c.rax));
    c.rdi = low(c.rdi + d.extra);
    if (d.extra) c.xmm[6] = {};
    c.rax = d.index;
    c.rcx = glyph(c.r14 + 0x120, d);
    test_flags(c, c.rcx);
}

void bitmap_split(Cpu& c) noexcept {
    const auto d = decode_glyph(text(c.rdx + c.rax));
    c.rax = d.index;
    load_float(c.xmm[6], c.r14 + 0x848);
    c.r15 = glyph(c.r14, d);
    test_flags(c, c.r15);
}

void bitmap_advance(Cpu& c) noexcept {
    c.rdi = low(next_glyph(engine_text(c.rbx), static_cast<std::uint32_t>(c.rdi)));
    c.rdx = c.rdi;
    c.r10 = at<std::uint32_t>(c.rbx + 0x10);
    compare_flags<std::uint32_t>(c, static_cast<std::uint32_t>(c.rdi), static_cast<std::uint32_t>(c.r10));
}

void split_format(Cpu& c) noexcept {
    c.r9 = at<std::uint64_t>(c.rbx + 0x18);
    c.rcx = c.r9 >= 0x10 ? at<std::uintptr_t>(c.rbx) : c.rbx;
    const auto f = format_glyph(text(c.rcx + c.rdx));
    c.rdi = low(c.rdi + f.extra);
    c.rdx += f.extra;
}

void list_format(Cpu& c) noexcept {
    c.r8 = at<std::uint64_t>(c.rdi + 0x18);
    c.rcx = c.r8 >= 0x10 ? at<std::uintptr_t>(c.rdi) : c.rdi;
    const auto f = format_glyph(text(c.rcx + c.rsi));
    c.rbx = low(c.rbx + f.extra);
    c.rsi = low(c.rsi + f.extra);
}

void list_measure(Cpu& c) noexcept {
    const auto d = decode_glyph(text(c.rax + c.rdx));
    c.rax = d.index;
    c.r8 = at<std::uintptr_t>(c.rbp - 0x48);
    load_float(c.xmm[6], c.r8 + 0x848);
    c.r12 = glyph(c.r8, d);
    test_flags(c, c.r12);
}

void list_advance(Cpu& c) noexcept {
    c.rbx = low(next_glyph(engine_text(c.rdi), static_cast<std::uint32_t>(c.rbx)));
    c.rsi = c.rbx;
    c.rcx = at<std::uint32_t>(c.rdi + 0x10);
    c.r9 = at<std::uint8_t>(c.rbp + 0x148);
}

void alternate_format(Cpu& c) noexcept {
    Glyph f{};
    if (!format_layout_glyph({text(c.rbx + c.rbp), static_cast<std::size_t>(c.r12 - c.rbx)}, f)) {
        c.rip = g.alternate_end;
        return;
    }
    if (f.index > 0xFF) {
        c.rip = g.alternate_plain_entry;
        return;
    }
    c.rbx += f.extra;
    c.rdi = low(c.rdi + f.extra);
    c.rax = f.index;
    compare_flags<std::uint8_t>(c, static_cast<std::uint8_t>(f.index), 0xA7);
}

void alternate_measure(Cpu& c) noexcept {
    Glyph d{};
    if (!layout_glyph({text(c.rbx + c.rbp), static_cast<std::size_t>(c.r12 - c.rbx)}, d)) {
        c.rip = g.alternate_end;
        return;
    }
    c.rbx += d.extra;
    c.rdi = low(c.rdi + d.extra);
    c.rdx = d.index;
    c.rcx = c.r15 + 0x120;
    c.r11 = glyph(c.rcx, d);
    test_flags(c, c.r11);
}

void alternate_advance(Cpu& c) noexcept {
    const auto next = next_glyph({text(c.rbp), static_cast<std::size_t>(c.r12)}, c.rbx);
    c.rdi = low(next);
    c.rbx = next;
}

// --- the tooltip routine (0x159C600) ---

// copy_popup_scalar: the character at `source` as a new string at `target`.
Glyph copy_popup_glyph(std::uintptr_t target, const char* source) noexcept {
    auto& s = at<EngineString>(target);
    s = {};
    s.capacity = 15;
    const auto d = decode_glyph(source);
    const std::uint64_t size = d.extra + 1;
    char* destination = g.repeat_text(&s, size, static_cast<std::uint8_t>(*source));
    std::memcpy(destination, source, size);
    return d;
}

void popup_copy(Cpu& c) noexcept { copy_popup_glyph(c.rbp + 0x50, text(c.rax + c.rdi)); }

void popup_color_copy(Cpu& c) noexcept {
    const auto d = copy_popup_glyph(c.rbp + 0x50, text(c.rax + c.rdi));
    c.rdi = low(c.rdi + d.extra);
}

void popup_icon_copy(Cpu& c) noexcept {
    const auto d = copy_popup_glyph(c.rbp + 0x50, text(c.rax + c.rdi));
    c.rbx = d.index;
    c.rdi = low(c.rdi + d.extra);
}

void popup_format(Cpu& c) noexcept {
    c.rax = at<std::uint64_t>(c.rsi + 0x18) >= 0x10 ? at<std::uintptr_t>(c.rsi) : c.rsi;
    const auto f = format_glyph(text(c.rax + c.rdi));
    if (f.index > 0xFF) {
        c.rip = g.popup_plain_entry;
        return;
    }
    c.rdi = low(c.rdi + f.extra);
    c.rcx = c.rsi;
    compare_flags<std::uint8_t>(c, at<std::uint8_t>(c.rax + c.rdi), 0xA7);
}

void popup_measure(Cpu& c) noexcept {
    const auto d = decode_glyph(text(c.rax + c.rdi));
    c.rdi = low(c.rdi + d.extra);
    c.rax = d.index;
    c.r12 = glyph(c.r15 + 0x120, d);
    test_flags(c, c.r12);
}

void popup_advance(Cpu& c) noexcept {
    c.rdi = low(next_glyph(engine_text(c.rsi), static_cast<std::uint32_t>(c.rdi)));
    compare_flags<std::uint32_t>(c, static_cast<std::uint32_t>(c.rdi), at<std::uint32_t>(c.rsi + 0x10));
}

void popup_draw_format(Cpu& c) noexcept {
    c.rax = c.r8 >= 0x10 ? c.r9 : c.rsp + 0x60;
    const auto f = format_glyph(text(c.rax + c.rsi));
    if (f.index > 0xFF) {
        c.rip = g.popup_draw_plain_entry;
        return;
    }
    c.rsi = low(c.rsi + f.extra);
    compare_flags<std::uint8_t>(c, at<std::uint8_t>(c.rax + c.rsi), 0xA7);
    c.rax = c.rsp + 0x60;
}

void popup_draw(Cpu& c) noexcept {
    const auto d = decode_glyph(text(c.rax + c.rsi));
    c.rsi = low(c.rsi + d.extra);
    c.rax = d.index;
    c.r13 = glyph(c.r15 + 0x120, d);
    test_flags(c, c.r13);
}

void popup_icon_end(Cpu& c) noexcept {
    c.rax = c.r8 >= 0x10 ? c.r9 : c.rsp + 0x60;
    if (icon_tail(c.rax + c.rsi)) c.rsi = low(c.rsi + 1);
    at<std::uint8_t>(c.rbp + c.rdx - 0x70) = 0;
}

// --- the font loader (0x1595570) ---
// It reads a font's glyph lines and keeps each glyph in a table of 256 pointers
// at font+0x120, after `cmp edi, 0xFF` drops any glyph above it. The original
// raises that limit to U+10FFFF and keeps the glyphs above 0xFF in its glyph
// registry (glyph_registry.cpp), keyed by the table, where the text blocks find
// them (font_hooks.asm).

std::uintptr_t font_table(Cpu& c) noexcept { return at<std::uintptr_t>(c.rbp + 0x1130) + 0x120; }

void font_lookup(Cpu& c) noexcept {
    const auto id = static_cast<std::uint32_t>(c.rdi);
    const auto table = font_table(c);
    c.rax = id <= 0xFF ? at<std::uintptr_t>(table + id * 8)
                       : reinterpret_cast<std::uintptr_t>(
                             eu4unicode::find_unicode_glyph(reinterpret_cast<void* const*>(table), id));
    test_flags(c, c.rax);
    c.rip = c.rax ? g.font_duplicate : g.font_allocate;
}

void font_allocate(Cpu& c) noexcept {
    const auto id = static_cast<std::uint32_t>(c.rdi);
    if (id <= 0xFF) {
        c.rcx = 0x10;
        c.rax = reinterpret_cast<std::uintptr_t>(g.engine_new(0x10));
        c.rip = g.font_initialize;
        return;
    }
    c.rax = reinterpret_cast<std::uintptr_t>(
        eu4unicode::allocate_unicode_glyph(reinterpret_cast<void* const*>(font_table(c)), id));
    test_flags(c, c.rax);
    c.rip = c.rax ? g.font_initialize : g.font_skip;
}

std::uint64_t g_failed_bindings = 0;

void font_store(Cpu& c) noexcept {
    c.r15 = at<std::uintptr_t>(c.rbp + 0x1130);
    const auto table = c.r15 + 0x120;
    const auto id = static_cast<std::uint32_t>(c.rdi);
    if (id > 0xFF) return;  // the registry already holds it
    at<std::uintptr_t>(table + id * 8) = c.rax;
    // 'A' names the font for the glyphs read before it (bind_unicode_font).
    if (id == 0x41 && !eu4unicode::bind_unicode_font(reinterpret_cast<void* const*>(table))) ++g_failed_bindings;
}

anvil::HookOf<sym::destroy_font_table> g_destroy_font;

void destroy_font_table(void* table) noexcept {
    eu4unicode::release_unicode_font(static_cast<void* const*>(table));
    g_destroy_font.next(table);
}

}  // namespace

std::size_t load_font_loader(anvil::Host& host, std::vector<anvil::Change>& changes) {
    const auto address = [&](const auto& symbol) { return reinterpret_cast<std::uintptr_t>(host.address(symbol)); };
    g.font_allocate = address(sym::font_allocate);
    g.font_duplicate = address(sym::font_duplicate);
    g.font_initialize = address(sym::font_initialize);
    g.font_skip = address(sym::font_skip);
    g.engine_new = reinterpret_cast<void* (*)(std::uint64_t)>(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr)) +
                                                           sym::font_allocate_block.engine_new);
    const auto start = changes.size();
    changes.push_back(host.patch(sym::font_codepoint_limit, {0xFF, 0xFF, 0x10, 0x00}));
    changes.push_back(host.replace_block(sym::font_lookup_block, font_lookup));
    changes.push_back(host.replace_block(sym::font_allocate_block, font_allocate));
    changes.push_back(host.replace_block(sym::font_store_block, font_store));
    host.hook(g_destroy_font, sym::destroy_font_table, destroy_font_table);
    return changes.size() - start + 1;
}

std::string font_loader_summary() {
    const auto usage = eu4unicode::unicode_glyph_usage();
    return std::format("font loader: {} glyphs above U+00FF in {} fonts{}", usage.glyphs, usage.fonts,
                       g_failed_bindings ? std::format(", {} fonts not bound", g_failed_bindings) : "");
}

std::size_t load_text_blocks(anvil::Host& host, std::vector<anvil::Change>& changes) {
    g.copy_buffer = host.data(sym::copy_buffer);
    g.repeat_text = host.function(sym::repeat_text);
    g.append_text = host.function(sym::append_text);
    const auto address = [&](const auto& symbol) { return reinterpret_cast<std::uintptr_t>(host.address(symbol)); };
    g.main_measure_entry = address(sym::main_measure_entry);
    g.main_plain_entry = address(sym::main_plain_entry);
    g.button_plain_entry = address(sym::button_plain_entry);
    g.button_draw_plain_entry = address(sym::button_draw_plain_entry);
    g.bitmap_plain_entry = address(sym::bitmap_plain_entry);
    g.button_loop = address(sym::button_loop);
    g.button_end = address(sym::button_end);
    g.alternate_end = address(sym::alternate_end);
    g.alternate_plain_entry = address(sym::alternate_plain_entry);
    g.popup_plain_entry = address(sym::popup_plain_entry);
    g.popup_draw_plain_entry = address(sym::popup_draw_plain_entry);

    const auto start = changes.size();
    const auto replace = [&](const auto& block, void (*callback)(Cpu&) noexcept) {
        changes.push_back(host.replace_block(block, callback));
    };
    replace(sym::main_copy_block, main_copy);
    replace(sym::main_measure_block, main_measure);
    replace(sym::main_format_block, main_format);
    replace(sym::main_icon_copy_block, main_icon_copy);
    replace(sym::main_icon_draw_block, main_icon_draw);
    replace(sym::main_draw_block, main_draw);
    replace(sym::button_copy_block, button_copy);
    replace(sym::button_format_block, button_format);
    replace(sym::button_icon_copy_block, button_icon_copy);
    replace(sym::button_measure_block, button_measure);
    replace(sym::button_advance_block, button_advance);
    replace(sym::button_draw_format_block, button_draw_format);
    replace(sym::button_icon_draw_block, button_icon_draw);
    replace(sym::button_draw_block, button_draw);
    replace(sym::text_limit_block, text_limit);
    replace(sym::bitmap_format_block, bitmap_format);
    replace(sym::bitmap_icon_end_block, bitmap_icon_end);
    replace(sym::bitmap_measure_block, bitmap_measure);
    replace(sym::bitmap_split_block, bitmap_split);
    replace(sym::bitmap_advance_block, bitmap_advance);
    replace(sym::split_format_block, split_format);
    replace(sym::list_format_block, list_format);
    replace(sym::list_measure_block, list_measure);
    replace(sym::list_advance_block, list_advance);
    replace(sym::alternate_format_block, alternate_format);
    replace(sym::alternate_measure_block, alternate_measure);
    replace(sym::alternate_advance_block, alternate_advance);
    replace(sym::popup_copy_block, popup_copy);
    replace(sym::popup_color_copy_block, popup_color_copy);
    replace(sym::popup_icon_copy_block, popup_icon_copy);
    replace(sym::popup_format_block, popup_format);
    replace(sym::popup_measure_block, popup_measure);
    replace(sym::popup_advance_block, popup_advance);
    replace(sym::popup_draw_format_block, popup_draw_format);
    replace(sym::popup_draw_block, popup_draw);
    replace(sym::popup_icon_end_block, popup_icon_end);
    return changes.size() - start;
}

}  // namespace unicode_patch
