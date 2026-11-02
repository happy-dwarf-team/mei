// The original's UTF-8 text input path (plugin.cpp: input_hook, dispatch_utf8,
// consume_editor_commit, insert_editor_commit) on Anvil's changes. SDL hands the
// game each typed or IME-committed text as UTF-8; the game keeps only what its
// own conversion allows. The original queues the whole text as one tagged
// event, lets the text field's character consumer take it in one piece and
// inserts it as the field's text, so Chinese, Japanese and Korean reach the field.
//
// Not ported yet (README.md "Next steps"): the editor's own steps by character
// (backspace, delete, arrows, selection, the width fit that drops the last
// byte), undo, caret alignment after a commit, the IME's candidate window and
// composition display. Until then those steps still work on bytes.
#include "text_input.hpp"

#include <windows.h>
#include <intrin.h>

#include <cstdint>
#include <cstring>
#include <string>

#include "symbols.gen.hpp"

#include "engine_string.hpp"
#include "native_text_event.hpp"
#include "unicode_editor.hpp"
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

// The commit being consumed: the field (its editor at -0x108 from the
// consumer) and the whole text that its insertion takes instead of one byte.
struct ActiveCommit {
    void* widget = nullptr;
    const EngineString* text = nullptr;
} g_active;

// The native callers the original singles out (plugin.cpp).
constexpr std::uintptr_t kQueuedCharacterCaller = 0x14E9CB6;  // the queue's type-2 consumer
constexpr std::uintptr_t kCommitInsertCaller = 0x1535615;     // consume_editor_commit's insertion

std::uintptr_t caller_rva(void* return_address) { return reinterpret_cast<std::uintptr_t>(return_address) - g_image; }

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

void insert(void* widget, const void* text) noexcept {
    if (caller_rva(_ReturnAddress()) == kCommitInsertCaller && g_active.widget == widget && g_active.text)
        text = g_active.text;
    g_insert.next(widget, text);
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
    return 3;
}

}  // namespace unicode_patch
