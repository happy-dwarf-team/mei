// The original's text field (plugin.cpp: input_hook, dispatch_utf8,
// consume_editor_commit, insert_editor_commit and the editor_* hooks) on Anvil's
// changes. SDL hands the game each typed or IME-committed text as UTF-8; the game
// keeps only what its own conversion allows. The original queues the whole text as
// one tagged event, lets the text field's character consumer take it in one piece
// and inserts it as the field's text, so Chinese, Japanese and Korean reach the field.
//
// A field whose text is UTF-8 then works by grapheme, as in the original without
// its shaped paragraphs and IME state: Backspace, Delete, the arrows, the mouse,
// selections, undo and redo, paste, the width fit, soft wraps and the caret after a
// commit. A field whose text is not UTF-8 (a save's name in Windows-1252) makes
// native_edit_view throw and keeps the game's own handling.
//
// Not ported (docs/port.md "Next steps"): the IME's composition display, and the
// original's selection boxes for fields of several rows and shaped paragraphs,
// which only those two need.
#include "text_input.hpp"

#include <windows.h>
#include <intrin.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>

#include "symbols.gen.hpp"

#include "editor_document.hpp"
#include "engine_string.hpp"
#include "game_text.hpp"
#include "native_editor_text.hpp"
#include "native_text_event.hpp"
#include "unicode_editor.hpp"
#include "unicode_services.hpp"
#include "unicode_text.hpp"

#pragma intrinsic(_ReturnAddress)

namespace unicode_patch {

namespace {

using eu4unicode::EngineString;

std::uintptr_t g_image = 0;
std::uintptr_t g_input_return = 0;
void* (*g_find_element)(void* set, const void* key) = nullptr;
const char* g_restricted = nullptr;
anvil::HookOf<sym::consume_editor_commit> g_consume;
anvil::HookOf<sym::insert_editor_commit> g_insert;
anvil::HookOf<sym::editor_key> g_key;
anvil::HookOf<sym::editor_left> g_left;
anvil::HookOf<sym::editor_right> g_right;
anvil::HookOf<sym::editor_selection> g_selection;
anvil::HookOf<sym::editor_point> g_point;
anvil::HookOf<sym::editor_width_fit> g_width_fit;
anvil::HookOf<sym::editor_word_break> g_word_break;
anvil::HookOf<sym::construct_editor_space_row> g_space_row;
anvil::HookOf<sym::paste_editor_clipboard> g_paste;
anvil::HookOf<sym::filter_editor_text> g_filter;
anvil::HookOf<sym::destroy_editor> g_destroy;
anvil::HookOf<sym::focus_editor_ime_rect> g_focus;
anvil::HookOf<sym::blur_editor_ime_rect> g_blur;
anvil::HookOf<sym::hide_editor> g_hide;
anvil::HookOf<sym::show_native_ime_candidates> g_window_message;
void* g_focused = nullptr;  // the text box with the keyboard, for window_message
using Assign = EngineString* (*)(EngineString*, const char*, std::uint64_t);
Assign g_assign = nullptr;
void (*g_string_destroy)(EngineString*) = nullptr;
void* (*g_caret_position)(void*, void*) = nullptr;
std::uint32_t (*g_row_offset)(void*, std::uint32_t, std::uint32_t) = nullptr;
void (*g_move_caret)(void*, std::uint32_t) = nullptr;
char* (*g_clipboard_text)() = nullptr;
void (*g_clipboard_free)(void*) = nullptr;

// The commit being consumed: the field (its editor at -0x108 from the
// consumer) and the whole text that its insertion takes instead of one byte.
struct ActiveCommit {
    void* widget = nullptr;
    const EngineString* text = nullptr;
} g_active;

// The native callers the original singles out (plugin.cpp).
// Steam's return addresses; caller_rva maps another build's to them (game_text.hpp).
constexpr std::uintptr_t kQueuedCharacterCaller = 0x14E9CB6;  // the queue's type-2 consumer
constexpr std::uintptr_t kCommitInsertCaller = 0x1535615;     // consume_editor_commit's insertion
constexpr std::uintptr_t kWriteFilterCaller = 0x1536C37;      // insert_editor_commit's refused characters
constexpr std::uintptr_t kFontFilterCaller = 0x15A05AD;       // the font's refused characters
// Tooltips and the lobby's chat wrap formatted text in temporary editors, which have
// no caret or history and take Windows-1252 commands.
constexpr std::uintptr_t kTooltipInsertCaller = 0x1415F38;
constexpr std::uintptr_t kLobbyChatInsertCaller = 0x83EAE0;

// The native text field's words (native_editor_text.cpp).
std::uint16_t& field_word(void* widget, std::size_t offset) {
    return *reinterpret_cast<std::uint16_t*>(static_cast<std::byte*>(widget) + offset);
}
constexpr std::size_t kCaretColumn = 0x54, kCaretRow = 0x56, kAnchorColumn = 0x92, kAnchorRow = 0x94;

std::uintptr_t caller_rva(void* return_address) {
    return unicode_patch::steam_caller(reinterpret_cast<std::uintptr_t>(return_address) - g_image);
}

// dispatch_utf8: the SDL text as one tagged event on the game's queue.
void dispatch_utf8(void* window, void* receiver, const char* payload, std::uint32_t event_value) noexcept {
    std::size_t length = 0;
    while (length < 32 && payload[length]) ++length;
    if (length == 32 || !eu4unicode::valid_utf8({payload, length})) return;
    eu4unicode::NativeTextEvent event{};
    if (!eu4unicode::make_text_event({payload, length}, event)) return;
    using WindowEvent = void (*)(void*, int, std::uint32_t, int);
    using TextEvent = void (*)(void*, eu4unicode::NativeTextEvent*);
    reinterpret_cast<WindowEvent>((*static_cast<void***>(window))[4])(window, 0x303, event_value, 0);
    reinterpret_cast<TextEvent>((*static_cast<void***>(receiver))[3])(receiver, &event);
}

// input_hook (0x1569F91): the SDL text event reaches dispatch_utf8 instead of the
// game's per-byte handling, which continues at input_return.
void input(anvil::Cpu& c) noexcept {
    dispatch_utf8(reinterpret_cast<void*>(c.r15), reinterpret_cast<void*>(c.r14),
                  reinterpret_cast<const char*>(c.rbp - 0x44), *reinterpret_cast<const std::uint32_t*>(c.rbp - 0x3C));
    c.rip = g_input_return;
}

std::string_view view(const EngineString& s) { return {s.data(), static_cast<std::size_t>(s.size)}; }

bool consume(void* callback, const char* character) noexcept {
    // This caller passes the type-2 queue node's text member, not an arbitrary
    // one-byte character pointer; other callers keep that ABI.
    if (caller_rva(_ReturnAddress()) != kQueuedCharacterCaller) return g_consume.next(callback, character);
    const auto* event = reinterpret_cast<const eu4unicode::NativeTextEvent*>(character - 0x10);
    if (event->utf8_tag != eu4unicode::native_utf8_tag) return g_consume.next(callback, character);
    const auto payload = eu4unicode::queued_utf8(*event);
    if (payload.empty()) return false;
    try {
        auto* base = static_cast<std::byte*>(callback);
        // The field's and its editor's refused characters, then the game's
        // context-dependent seven-character restriction.
        auto filtered = eu4unicode::filter_editor_characters(payload, view(*reinterpret_cast<const EngineString*>(base + 0xD0)));
        filtered = eu4unicode::filter_editor_characters(filtered, view(*reinterpret_cast<const EngineString*>(base - 0x50)));
        if (g_find_element(*reinterpret_cast<void**>(base + 0x10), base + 0x30) &&
            *reinterpret_cast<const unsigned char*>(base - 0x24))
            filtered = eu4unicode::filter_editor_characters(filtered, {g_restricted, 7});
        if (filtered.empty()) return false;
        EngineString text{};
        text.size = filtered.size();
        text.capacity = filtered.size() < 16 ? 15 : filtered.size();
        if (text.capacity < 16)
            std::memcpy(text.storage.inline_bytes, filtered.c_str(), filtered.size() + 1);
        else
            text.storage.pointer = filtered.c_str();
        const auto previous = g_active;
        struct Restore {
            ActiveCommit previous;
            ~Restore() { g_active = previous; }
        } restore{previous};
        g_active = {base - 0x108, &text};
        // The native consumer keeps its notification and focus logic; its one
        // insertion call gets the whole text (insert below).
        return g_consume.next(callback, filtered.c_str());
    } catch (...) {
        return false;
    }
}

// A field's undo history. Its changes are whole keys and commits; a transaction
// records one when it ends, unless the field has gone (a key can close it).
struct History {
    eu4unicode::EditHistory history;
    eu4unicode::EditState before;
    unsigned depth = 0;
    bool alive = true;
};
std::unordered_map<void*, std::shared_ptr<History>> g_histories;

struct Transaction {
    void* widget;
    std::shared_ptr<History> entry;
    explicit Transaction(void* owner) : widget(owner) {
        try {
            auto& stored = g_histories[owner];
            if (!stored) stored = std::make_shared<History>();
            entry = stored;
            if (!entry->depth) entry->before = eu4unicode::native_edit_state(widget);
            ++entry->depth;
        } catch (...) {
            entry.reset();
        }
    }
    ~Transaction() {
        if (!entry || --entry->depth || !entry->alive) return;
        try {
            entry->history.record(entry->before, eu4unicode::native_edit_state(widget));
        } catch (...) {
            entry->history.clear();
        }
    }
};

constexpr std::size_t kFieldOfBox = 0xC8;  // the text field in its box

void focus(void* outer) noexcept {
    g_focus.next(outer);
    if (static_cast<const std::byte*>(outer)[0x260] == std::byte{1}) g_focused = outer;
}
void blur(void* outer) noexcept {
    g_blur.next(outer);
    if (g_focused == outer) g_focused = nullptr;
}
void hide(void* outer) noexcept {
    if (g_focused == outer) g_focused = nullptr;
    g_hide.next(outer);
}

void destroy(void* outer) noexcept {
    if (g_focused == outer) g_focused = nullptr;
    const auto found = g_histories.find(static_cast<std::byte*>(outer) + kFieldOfBox);
    if (found != g_histories.end()) {
        found->second->alive = false;
        g_histories.erase(found);
    }
    g_destroy.next(outer);
}

// editor_selection: a selection takes in every grapheme it touches, whichever way it runs.
void selection(void* widget) noexcept {
    auto* base = static_cast<std::byte*>(widget);
    try {
        const auto view = eu4unicode::native_edit_view(widget);
        const auto anchor = view.rows.offset({field_word(widget, kAnchorRow), field_word(widget, kAnchorColumn)});
        const auto caret = view.rows.offset({field_word(widget, kCaretRow), field_word(widget, kCaretColumn)});
        const auto aligned = eu4unicode::align_selection(view.text, anchor, caret);
        const auto first = view.rows.position(aligned.anchor), last = view.rows.position(aligned.caret);
        field_word(widget, kAnchorColumn) = static_cast<std::uint16_t>(first.column);
        field_word(widget, kAnchorRow) = static_cast<std::uint16_t>(first.row);
        field_word(widget, kCaretColumn) = static_cast<std::uint16_t>(last.column);
        field_word(widget, kCaretRow) = static_cast<std::uint16_t>(last.row);
        *reinterpret_cast<std::uint32_t*>(base + 0x50) = static_cast<std::uint32_t>(last.column);
    } catch (...) {
    }
    g_selection.next(widget);
}

void insert(void* widget, const void* text) noexcept {
    const auto caller = caller_rva(_ReturnAddress());
    if (caller == kTooltipInsertCaller || caller == kLobbyChatInsertCaller) return g_insert.next(widget, text);
    Transaction transaction(widget);
    if (caller == kCommitInsertCaller && g_active.widget == widget && g_active.text) text = g_active.text;
    if (static_cast<std::byte*>(widget)[0x90] != std::byte{0}) selection(widget);
    g_insert.next(widget, text);
    if (transaction.entry && !transaction.entry->alive) return;
    try {
        eu4unicode::native_edit_align_commit(widget);
    } catch (...) {
    }
}

// The original's filter_editor_text: the refused characters are single Latin-1
// bytes, which the game's filter would take out of UTF-8 sequences.
thread_local bool g_clipboard_font_filter = false;
void filter(void* raw, const void* raw_refused) noexcept {
    const auto caller = caller_rva(_ReturnAddress());
    auto* text = static_cast<EngineString*>(raw);
    const auto value = view(*text);
    if ((caller != kWriteFilterCaller && !(g_clipboard_font_filter && caller == kFontFilterCaller)) ||
        !eu4unicode::valid_utf8(value))
        return g_filter.next(raw, raw_refused);
    try {
        const auto filtered = eu4unicode::filter_editor_characters(value, view(*static_cast<const EngineString*>(raw_refused)));
        auto* data = const_cast<char*>(text->data());
        std::memcpy(data, filtered.data(), filtered.size());
        data[filtered.size()] = '\0';
        text->size = filtered.size();
    } catch (...) {
        text->size = 0;
        const_cast<char*>(text->data())[0] = '\0';
    }
}

// The three places where the field cuts a text to a byte limit: rcx, rdx and r8 of
// the game's std::string assign, the limit moved back to a whole grapheme.
void assign_prefix(anvil::Cpu& c) noexcept {
    const auto* source = reinterpret_cast<const char*>(c.rdx);
    auto length = static_cast<std::uint64_t>(c.r8);
    const auto complete = std::string_view(source, std::strlen(source));
    if (eu4unicode::valid_utf8(complete)) {
        try {
            length = eu4unicode::grapheme_prefix(complete, static_cast<std::size_t>(length));
        } catch (...) {
            length = 0;
        }
    }
    c.rax = reinterpret_cast<std::uintptr_t>(g_assign(reinterpret_cast<EngineString*>(c.rcx), source, length));
}

// The original's trim_editor_grapheme: the width fit that would drop a text's last
// byte drops its last grapheme.
void fit(anvil::Cpu& c) noexcept {
    auto* widget = reinterpret_cast<void*>(c.rdi);
    auto* base = static_cast<std::byte*>(widget);
    auto* text = reinterpret_cast<EngineString*>(base + 0x30);
    const auto value = view(*text);
    if (eu4unicode::valid_utf8(value) && !value.empty()) {
        const auto position = g_row_offset(widget, field_word(widget, kCaretRow), field_word(widget, kCaretColumn));
        if (position && position <= value.size()) {
            try {
                const auto plan = eu4unicode::plan_edit(value, position, eu4unicode::EditKey::backspace);
                auto* data = const_cast<char*>(text->data());
                std::memmove(data + plan.erase_begin, data + plan.erase_end, value.size() - plan.erase_end + 1);
                text->size -= plan.erase_end - plan.erase_begin;
                base[0x100] = base[0x101] = std::byte{1};
                g_move_caret(widget, static_cast<std::uint32_t>(plan.caret));
                struct Rows {
                    const std::byte* begin;
                    const std::byte* end;
                };
                const auto* rows = reinterpret_cast<const Rows* (*)(void*)>((*static_cast<void***>(widget))[0x1A0 / 8])(widget);
                field_word(widget, 0x60) = static_cast<std::uint16_t>((rows->end - rows->begin) / 40);
                return;
            } catch (...) {
            }
        }
    }
    // The game's own step for text that is not UTF-8 and for an empty position.
    reinterpret_cast<void (*)(void*)>((*static_cast<void***>(widget))[0x138 / 8])(widget);
}

// How wide a prefix of a row's text is, in the field's font.
eu4unicode::PrefixMeasure measure_of(void* widget, std::string_view text) {
    const auto* base = static_cast<const std::byte*>(widget);
    auto* font = *reinterpret_cast<void* const*>(base + 0x98);
    const auto flags = *reinterpret_cast<const unsigned char*>(base + 0xE4);
    using Measure = int (*)(void*, const char*, int, unsigned char);
    const auto measure = reinterpret_cast<Measure>((*static_cast<void***>(font))[0x60 / 8]);
    return [font, flags, measure, text](std::size_t length) {
        const std::string prefix(text.substr(0, length));
        return measure(font, prefix.c_str(), static_cast<int>(length), flags);
    };
}

struct Point {
    int x, y;
};

// editor_point: a click lands on the nearest grapheme boundary.
void point(void* widget, const void* row, const void* raw) noexcept {
    const auto* where = static_cast<const Point*>(raw);
    if (where->x < 0 || where->y < 0) return g_point.next(widget, row, raw);
    try {
        const auto view = eu4unicode::native_edit_view(widget);
        auto* base = static_cast<std::byte*>(widget);
        auto* font = *reinterpret_cast<void**>(base + 0x98);
        const auto height = reinterpret_cast<int (*)(void*)>((*static_cast<void***>(font))[0x68 / 8])(font);
        if (height <= 0) return g_point.next(widget, row, raw);
        const auto index = base[0xD9] != std::byte{0}
                               ? (std::min)(static_cast<std::size_t>(where->y / height), view.rows.rows().size() - 1)
                               : std::size_t{0};
        const auto& line = view.rows.rows()[index];
        const auto value = view.text.substr(line.start, line.length);
        const auto hit = eu4unicode::nearest_grapheme_boundary(value, where->x, measure_of(widget, value));
        field_word(widget, kCaretColumn) = static_cast<std::uint16_t>(hit);
        field_word(widget, kCaretRow) = static_cast<std::uint16_t>(index);
    } catch (...) {
        g_point.next(widget, row, raw);
    }
}

// editor_width_fit: how many bytes of a text fit a row, in whole graphemes.
int width_fit(void* widget, const void* raw) noexcept {
    const auto* text = static_cast<const EngineString*>(raw);
    const auto value = view(*text);
    if (text->size > 32000 || !eu4unicode::valid_utf8(value)) return g_width_fit.next(widget, raw);
    auto line_end = value.find('\n');
    if (line_end != std::string_view::npos && line_end && value[line_end - 1] == '\r') --line_end;
    const auto line = value.substr(0, line_end);
    try {
        if (line.empty()) return 0;
        const auto* base = static_cast<const std::byte*>(widget);
        auto* font = *reinterpret_cast<void* const*>(base + 0x98);
        const auto margin = reinterpret_cast<int (*)(void*, unsigned char)>((*static_cast<void***>(font))[0xA8 / 8])(font, 1);
        const auto width = static_cast<int>(*reinterpret_cast<const std::uint16_t*>(base + 0x68)) - margin;
        const auto fitting = eu4unicode::fitting_grapheme_prefix(line, width, measure_of(widget, line));
        // An oversized first grapheme is taken whole, so the game's row builder
        // goes on without splitting it.
        return static_cast<int>(fitting ? fitting : eu4unicode::grapheme_boundaries(line)[1]);
    } catch (...) {
        return static_cast<int>(line.size());
    }
}

// editor_word_break: a soft wrap takes the whole grapheme of the space it ends at.
int word_break(void* widget, const void* raw, int fitting) noexcept {
    const auto result = g_word_break.next(widget, raw, fitting);
    const auto* text = static_cast<const EngineString*>(raw);
    const auto value = view(*text);
    if (result < 0 || text->size > 32000 || !eu4unicode::valid_utf8(value)) return result;
    try {
        const auto boundaries = eu4unicode::grapheme_boundaries(value);
        const auto end = std::lower_bound(boundaries.begin(), boundaries.end(), static_cast<std::size_t>(result) + 1);
        return static_cast<int>(*end) - 1;
    } catch (...) {
        return -1;
    }
}

// construct_editor_space_row, the original's, with the game's function as its next step.
EngineString* native_space_row(EngineString* row, const EngineString* source) {
    return static_cast<EngineString*>(g_space_row.next(row, source));
}
void* space_row(void* row, const void* source) noexcept {
    try {
        return eu4unicode::construct_editor_space_row(static_cast<EngineString*>(row), static_cast<const EngineString*>(source));
    } catch (...) {
        return g_space_row.next(row, source);
    }
}

// paste_editor_clipboard: the clipboard's UTF-8 as one commit (the game converts it
// and drops what Windows-1252 has not); rejected text leaves the field as it is.
void paste(void* widget) noexcept {
    const std::unique_ptr<char, void (*)(void*)> clipboard(g_clipboard_text(), g_clipboard_free);
    if (!clipboard) return;
    const auto length = strnlen_s(clipboard.get(), 32001);
    if (!length || length > 32000) return;
    const std::string_view value(clipboard.get(), length);
    if (!eu4unicode::valid_utf8(value)) return;
    try {
        EngineString text{};
        text.capacity = 15;
        struct DestroyText {
            EngineString* text;
            ~DestroyText() { g_string_destroy(text); }
        } destroy_text{&text};
        g_assign(&text, value.data(), value.size());
        const auto* base = static_cast<const std::byte*>(widget);
        auto* font = *reinterpret_cast<void* const*>(base + 0x98);
        if (font && base[0xE4] != std::byte{0}) {
            struct Restore {
                bool previous;
                ~Restore() { g_clipboard_font_filter = previous; }
            } restore{g_clipboard_font_filter};
            g_clipboard_font_filter = true;
            reinterpret_cast<void (*)(void*, EngineString*)>((*static_cast<void***>(font))[0xC0 / 8])(font, &text);
        }
        const auto filtered =
            eu4unicode::filter_editor_characters(view(text), view(*reinterpret_cast<const EngineString*>(base + 0xB8)));
        if (filtered.empty()) return;
        g_assign(&text, filtered.data(), filtered.size());
        insert(widget, &text);
    } catch (...) {
    }
}

// The game's key event for a text field: SDL key code and modifiers (1 Ctrl,
// 4 Shift).
struct KeyEvent {
    std::uint32_t key, unused, modifiers;
};

void redraw(void* widget) {
    reinterpret_cast<void (*)(void*)>((*static_cast<void***>(widget))[0x208 / 8])(widget);
}

// Up and down in a field of several rows: the nearest boundary of the next row.
void vertical(void* widget, bool down, bool extend) {
    const auto view = eu4unicode::native_edit_view(widget);
    auto* base = static_cast<std::byte*>(widget);
    const auto row = field_word(widget, kCaretRow);
    if ((down && row + 1u >= view.rows.rows().size()) || (!down && !row)) return;
    struct PixelPoint {
        std::uint16_t x, y;
    } pixel{};
    g_caret_position(widget, &pixel);
    const auto* font = *reinterpret_cast<const std::byte* const*>(base + 0x98);
    const auto x = (std::max)(0, static_cast<int>(pixel.x) - *reinterpret_cast<const int*>(font + 0x38));
    const auto next = down ? row + 1 : row - 1;
    const auto& line = view.rows.rows()[next];
    const auto value = view.text.substr(line.start, line.length);
    const auto hit = eu4unicode::nearest_grapheme_boundary(value, x, measure_of(widget, value));
    if (extend && base[0x90] == std::byte{0}) *reinterpret_cast<std::uint32_t*>(base + 0x92) = *reinterpret_cast<std::uint32_t*>(base + 0x54);
    eu4unicode::native_edit_caret(widget, line.start + hit, !extend);
    field_word(widget, kCaretRow) = static_cast<std::uint16_t>(next);
    field_word(widget, kCaretColumn) = static_cast<std::uint16_t>(hit);
    if (extend) {
        selection(widget);
        base[0x90] = std::byte{1};
    }
    redraw(widget);
}

constexpr std::uint32_t kKeyRight = 0x4000004F, kKeyLeft = 0x40000050, kKeyDown = 0x40000051, kKeyUp = 0x40000052;
constexpr std::uint32_t kCtrl = 1, kShift = 4;

// The original's editor_key without its IME state. A field whose text is not UTF-8
// throws in native_edit_state and keeps the game's own handling.
bool key(void* widget, const void* raw) noexcept {
    const auto* event = static_cast<const KeyEvent*>(raw);
    try {
        if ((event->key == 'a' || event->key == 'A') && event->modifiers == kCtrl) {
            eu4unicode::native_edit_select_all(widget);
            return true;
        }
        if ((event->key == 'z' || event->key == 'Z') && (event->modifiers == kCtrl || event->modifiers == (kCtrl | kShift))) {
            const auto found = g_histories.find(widget);
            if (found != g_histories.end()) {
                const auto state = eu4unicode::native_edit_state(widget);
                const auto target =
                    event->modifiers == kCtrl ? found->second->history.undo(state) : found->second->history.redo(state);
                if (target) eu4unicode::native_edit_restore(widget, *target);
            }
            return true;
        }
        Transaction transaction(widget);
        const auto state = eu4unicode::native_edit_state(widget);
        const auto* base = static_cast<const std::byte*>(widget);
        if (base[0x90] != std::byte{0}) selection(widget);
        if (event->modifiers == kShift && (event->key == kKeyLeft || event->key == kKeyRight)) {
            // The game's move that extends the selection, which goes through the arrows below.
            reinterpret_cast<void (*)(void*)>((*static_cast<void***>(widget))[(event->key == kKeyLeft ? 0xD0 : 0xE0) / 8])(widget);
            return true;
        }
        if ((event->modifiers == 0 || event->modifiers == kShift) && base[0xD9] != std::byte{0} &&
            (event->key == kKeyDown || event->key == kKeyUp)) {
            if (base[0x102] != std::byte{0}) return false;
            vertical(widget, event->key == kKeyDown, event->modifiers == kShift);
            return true;
        }
        if (!event->modifiers && (event->key == 8 || event->key == 127)) {
            eu4unicode::EditResult result;
            if (state.selection.anchor != state.selection.caret) {
                result = eu4unicode::replace_selection(state.text, state.selection.anchor, state.selection.caret, "", 32000);
            } else {
                const auto plan = eu4unicode::plan_edit(
                    state.text, state.selection.caret,
                    event->key == 8 ? eu4unicode::EditKey::backspace : eu4unicode::EditKey::forward_delete);
                result = {state.text, plan.caret};
                result.text.erase(plan.erase_begin, plan.erase_end - plan.erase_begin);
            }
            if (result.text != state.text)
                eu4unicode::native_edit_restore(widget, {std::move(result.text), {result.caret, result.caret}});
            return true;
        }
        return g_key.next(widget, raw);
    } catch (...) {
        return g_key.next(widget, raw);
    }
}

// The game's dispatch can consume Ctrl+Shift+Z before a field sees it, so the
// original takes history keys in SDL's window message path while a field has the
// keyboard (show_native_ime_candidates; the rest of it is IME state).
int window_message(void* window, std::uint32_t message, std::uint64_t parameter, void* flags, void* video) noexcept {
    const auto scan = flags ? *static_cast<const std::int64_t*>(flags) : 0;
    const auto virtual_key = parameter == VK_PROCESSKEY ? MapVirtualKeyW(static_cast<UINT>((scan >> 16) & 0xFF), MAPVK_VSC_TO_VK)
                                                        : static_cast<UINT>(parameter);
    const int trapped = g_window_message.next(window, message, parameter, flags, video);
    if (message != WM_KEYDOWN || virtual_key != 'Z' || !g_focused || !(GetKeyState(VK_CONTROL) & 0x8000) ||
        (GetKeyState(VK_MENU) & 0x8000) || (GetKeyState(VK_LWIN) & 0x8000) || (GetKeyState(VK_RWIN) & 0x8000))
        return trapped;
    const KeyEvent event{'z', 0, (GetKeyState(VK_SHIFT) & 0x8000) ? kCtrl | kShift : kCtrl};
    key(static_cast<std::byte*>(g_focused) + kFieldOfBox, &event);
    return 1;
}

// editor_arrow: the caret moves one grapheme, across rows too.
template <bool Right> void arrow(void* widget) noexcept {
    try {
        const auto view = eu4unicode::native_edit_view(widget);
        const auto offset = view.rows.offset({field_word(widget, kCaretRow), field_word(widget, kCaretColumn)});
        const auto target =
            eu4unicode::plan_edit(view.text, offset, Right ? eu4unicode::EditKey::right : eu4unicode::EditKey::left).caret;
        eu4unicode::native_edit_caret(widget, target);
        redraw(widget);
    } catch (...) {
        (Right ? g_right : g_left).next(widget);
    }
}

}  // namespace

std::size_t load_text_input(anvil::Host& host, std::vector<anvil::Change>& changes) {
    g_image = reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));
    g_input_return = reinterpret_cast<std::uintptr_t>(host.address(sym::input_return));
    g_find_element = host.function(sym::find_element);
    g_restricted = host.data(sym::editor_restricted_characters);
    changes.push_back(host.replace_block(sym::input_block, input));
    host.hook(g_consume, sym::consume_editor_commit, consume);
    host.hook(g_insert, sym::insert_editor_commit, insert);
    g_assign = reinterpret_cast<Assign>(host.function(sym::assign_editor_prefix));
    g_string_destroy = reinterpret_cast<void (*)(EngineString*)>(host.function(sym::string_destroy));
    g_move_caret = host.function(sym::editor_move_caret);
    g_row_offset = host.function(sym::editor_row_offset);
    g_clipboard_text = host.function(sym::clipboard_text);
    g_clipboard_free = host.function(sym::clipboard_free);
    eu4unicode::configure_native_editor_text(eu4unicode::NativeEditorTextApi{g_assign, g_string_destroy, g_move_caret});
    eu4unicode::original_editor_space_row = native_space_row;
    host.hook(g_key, sym::editor_key, key);
    host.hook(g_left, sym::editor_left, arrow<false>);
    host.hook(g_right, sym::editor_right, arrow<true>);
    host.hook(g_selection, sym::editor_selection, selection);
    host.hook(g_point, sym::editor_point, point);
    host.hook(g_width_fit, sym::editor_width_fit, width_fit);
    host.hook(g_word_break, sym::editor_word_break, word_break);
    host.hook(g_space_row, sym::construct_editor_space_row, space_row);
    host.hook(g_paste, sym::paste_editor_clipboard, paste);
    host.hook(g_filter, sym::filter_editor_text, filter);
    host.hook(g_destroy, sym::destroy_editor, destroy);
    host.hook(g_focus, sym::focus_editor_ime_rect, focus);
    host.hook(g_blur, sym::blur_editor_ime_rect, blur);
    host.hook(g_hide, sym::hide_editor, hide);
    host.hook(g_window_message, sym::show_native_ime_candidates, window_message);
    g_caret_position = host.function(sym::editor_caret_position);
    changes.push_back(host.replace_block(sym::editor_fit_block, fit));
    changes.push_back(host.replace_block(sym::prefix_assign_block_153a306, assign_prefix));
    changes.push_back(host.replace_block(sym::prefix_assign_block_153a717, assign_prefix));
    changes.push_back(host.replace_block(sym::prefix_assign_block_153a99b, assign_prefix));
    // SDL's IME set-up adds a UI-less TSF manager, which asks the game to draw the
    // candidates itself (it does not); the original jumps past it, so the IME's own
    // candidate window shows: jmp rel32 to 0x1764F82, then two NOPs.
    changes.push_back(host.patch(sym::sdl_ime_ui_less, {0xE9, 0x27, 0x01, 0x00, 0x00, 0x90, 0x90}));
    return 23;
}

}  // namespace unicode_patch
