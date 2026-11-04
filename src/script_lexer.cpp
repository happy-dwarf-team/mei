#include "script_lexer.hpp"

#include "symbols.gen.hpp"

#include "localisation.hpp"
#include "native_legacy_import.hpp"
#include "native_script_bom.hpp"

namespace unicode_patch {

namespace {

using eu4unicode::NativeScriptLexer;

anvil::HookOf<sym::construct_script_file> g_file;
anvil::HookOf<sym::construct_script_file_mode> g_file_mode;
anvil::HookOf<sym::construct_script_stream> g_stream;
anvil::HookOf<sym::read_legacy_script_token> g_script_token;
anvil::HookOf<sym::read_legacy_binary_token> g_binary_token;

void* skip_signature(void* lexer) noexcept {
    eu4unicode::consume_script_bom(static_cast<NativeScriptLexer*>(lexer)->input);
    return lexer;
}
void* construct_file(void* lexer, void* path) noexcept { return skip_signature(g_file.next(lexer, path)); }
void* construct_file_mode(void* lexer, void* path, int mode) noexcept {
    return skip_signature(g_file_mode.next(lexer, path, mode));
}
void* construct_stream(void* lexer, void* input, bool own_input) noexcept {
    return skip_signature(g_stream.next(lexer, input, own_input));
}

// As in the original, a token it rejects ends the file or save for the game.
int read_script_token(void* raw) noexcept {
    const int result = g_script_token.next(raw);
    auto* lexer = static_cast<NativeScriptLexer*>(raw);
    if (!result || !token_has_legacy_escape(lexer->text, sizeof lexer->text)) return result;
    return eu4unicode::convert_legacy_script_token(lexer) ? result : 0;
}
int read_binary_token(void* raw) noexcept {
    const int result = g_binary_token.next(raw);
    auto* lexer = static_cast<NativeScriptLexer*>(raw);
    if (!result || !token_has_legacy_escape(lexer->text, sizeof lexer->text)) return result;
    return eu4unicode::convert_legacy_binary_token(lexer) ? result : 0;
}

}  // namespace

std::size_t load_script_lexer(anvil::Host& host) {
    host.hook(g_file, sym::construct_script_file, construct_file);
    host.hook(g_file_mode, sym::construct_script_file_mode, construct_file_mode);
    host.hook(g_stream, sym::construct_script_stream, construct_stream);
    host.hook(g_script_token, sym::read_legacy_script_token, read_script_token);
    host.hook(g_binary_token, sym::read_legacy_binary_token, read_binary_token);
    host.subscribe("anvil-game.ready", [] { anvil::Host{}.info("{}", lexer_summary()); });
    // A session's save has been read.
    host.subscribe("anvil-game.session", [] { anvil::Host{}.info("{}", lexer_summary()); });
    return 5;
}

}  // namespace unicode_patch
