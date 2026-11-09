#include "dates_titles.hpp"

#include <cstddef>
#include <string>
#include <string_view>

#include "symbols.gen.hpp"

#include "cjk_format.hpp"
#include "engine_string.hpp"

namespace unicode_patch {

namespace {

using eu4unicode::EngineString;

using DateFormat = void*(const void* date, void* result, const void* format);
using DateJoin = void*(void* result, const void* month, const void* year);

anvil::FunctionHook<DateFormat> g_topbar_date;
anvil::FunctionHook<DateJoin> g_end_of_month, g_claim_strength, g_rebel, g_month_year, g_game_setup;
anvil::HookOf<sym::format_localized_battle_title> g_combat_title;
sym::Types::assign_editor_prefix::Pointer g_assign = nullptr;

std::string_view text(const void* value) {
    const auto* s = static_cast<const EngineString*>(value);
    return {s->data(), static_cast<std::size_t>(s->size)};
}

void assign(void* target, const std::string& value) {
    if (!value.empty()) g_assign(target, value.data(), value.size());
}

// The top bar's "d w mw w y": "11 11月 1444".
void* topbar_date(const void* date, void* result, const void* format) noexcept {
    void* output = g_topbar_date.next(date, result, format);
    try {
        const std::string_view value = text(output);
        const auto first = value.find(' '), last = value.rfind(' ');
        if (first != std::string_view::npos && last > first)
            assign(output, cjk_date(value.substr(0, first), value.substr(first + 1, last - first - 1),
                                    value.substr(last + 1)));
    } catch (...) {
        // The game's text stays.
    }
    return output;
}

// A month ("11月 ," or "11月 ") joined to a year.
template <anvil::FunctionHook<DateJoin>& Hook>
void* month_year(void* result, const void* month, const void* year) noexcept {
    std::string replacement;
    try {
        replacement = cjk_month_year(text(month), text(year));
    } catch (...) {
    }
    void* output = Hook.next(result, month, year);
    try {
        assign(output, replacement);
    } catch (...) {
    }
    return output;
}

// CCombat::GetName: a localised prefix, a space and the place, which the game reads
// from _pLocation (0x30), its _Template (0x28) and the name there (0x10), as the original.
void* combat_title(const void* combat, void* result) noexcept {
    void* output = g_combat_title.next(combat, result);
    try {
        const auto* location = *reinterpret_cast<const std::byte* const*>(static_cast<const std::byte*>(combat) + 0x30);
        if (location == nullptr) return output;
        const auto* province = *reinterpret_cast<const std::byte* const*>(location + 0x28);
        const std::string_view place = text(province + 0x10);
        const std::string_view value = text(output);
        if (!place.empty() && value.size() > place.size() && value.ends_with(place))
            assign(output, cjk_combat_title(value.substr(0, value.size() - place.size()), place));
    } catch (...) {
    }
    return output;
}

}  // namespace

std::size_t load_dates_titles(anvil::Host& host) {
    g_assign = host.function(sym::assign_editor_prefix);
    host.redirect(g_topbar_date, sym::topbar_date_call_1432256, topbar_date);
    host.redirect(g_end_of_month, sym::end_of_month_date_call_6e5cef, month_year<g_end_of_month>);
    host.redirect(g_month_year, sym::month_year_date_call_6e61b9, month_year<g_month_year>);
    host.redirect(g_claim_strength, sym::claim_strength_date_call_a52956, month_year<g_claim_strength>);
    host.redirect(g_rebel, sym::rebel_date_call_12ba0e6, month_year<g_rebel>);
    host.redirect(g_game_setup, sym::game_setup_date_call_1133a58, month_year<g_game_setup>);
    host.hook(g_combat_title, sym::format_localized_battle_title, combat_title);
    return 7;
}

}  // namespace unicode_patch
