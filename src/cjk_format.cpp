#include "cjk_format.hpp"

#include <algorithm>
#include <cstdint>

#include "localized_format.hpp"

namespace unicode_patch {

namespace {

// The words of a Korean date: year, month and day.
struct KoreanWords {
    std::string_view year;
    std::string_view month;
    std::string_view day;
};

// In Hangul: 년, 월, 일.
constexpr KoreanWords hangul_words{"\xEB\x85\x84", "\xEC\x9B\x94", "\xEC\x9D\xBC"};
// As the Korean Language Mod (Workshop 2714934430) writes them: it draws Hangul from Latin-1
// letters of its own fonts, so its 년, 월 and 일 are "¥ÀÒ", "±ÈÖ" and "±ÎÖ" (in UTF-8 here).
constexpr KoreanWords latin_pair_words{"\xC2\xA5\xC3\x80\xC3\x92", "\xC2\xB1\xC3\x88\xC3\x96",
                                       "\xC2\xB1\xC3\x8E\xC3\x96"};

// The words a Korean month is written with, or none.
const KoreanWords* korean_words(std::string_view month) {
    for (const KoreanWords* words : {&hangul_words, &latin_pair_words}) {
        if (month.ends_with(words->month) && month.size() > words->month.size()) return words;
    }
    return nullptr;
}

std::string_view trim(std::string_view value) {
    while (!value.empty() && (value.front() == ' ' || value.front() == ',')) value.remove_prefix(1);
    while (!value.empty() && (value.back() == ' ' || value.back() == ',')) value.remove_suffix(1);
    return value;
}

bool number(std::string_view value) {
    return !value.empty() && std::all_of(value.begin(), value.end(), [](char c) { return c >= '0' && c <= '9'; });
}

// The first character of valid UTF-8 text, or 0.
char32_t first_scalar(std::string_view text) {
    if (text.empty()) return 0;
    const auto b = [&](std::size_t i) { return static_cast<unsigned char>(text[i]); };
    const unsigned char lead = b(0);
    if (lead < 0x80) return lead;
    const std::size_t length = lead >= 0xF0 ? 4 : lead >= 0xE0 ? 3 : lead >= 0xC0 ? 2 : 0;
    if (length == 0 || text.size() < length) return 0;
    char32_t value = lead & (0x7F >> length);
    for (std::size_t i = 1; i < length; ++i) {
        if ((b(i) & 0xC0) != 0x80) return 0;
        value = (value << 6) | (b(i) & 0x3F);
    }
    return value;
}

bool hangul(char32_t c) {
    return (c >= 0x1100 && c <= 0x11FF) || (c >= 0x3130 && c <= 0x318F) || (c >= 0xAC00 && c <= 0xD7AF);
}

bool han_or_kana(char32_t c) {
    return (c >= 0x3040 && c <= 0x30FF) || (c >= 0x3400 && c <= 0x4DBF) || (c >= 0x4E00 && c <= 0x9FFF) ||
           (c >= 0xF900 && c <= 0xFAFF) || (c >= 0x20000 && c <= 0x3134F);
}

std::string korean_month_year(const KoreanWords& words, std::string_view month, std::string_view year) {
    return std::string(year) + std::string(words.year) + " " + std::string(month);
}

}  // namespace

std::string cjk_date(std::string_view day, std::string_view month, std::string_view year) {
    if (auto chinese = eu4unicode::chinese_date(day, month, year); !chinese.empty()) return chinese;
    day = trim(day);
    month = trim(month);
    year = trim(year);
    const KoreanWords* words = korean_words(month);
    if (words == nullptr || !number(year) || !number(day)) return {};
    return korean_month_year(*words, month, year) + " " + std::string(day) + std::string(words->day);
}

std::string cjk_month_year(std::string_view month, std::string_view year) {
    if (auto chinese = eu4unicode::chinese_month_year(month, year); !chinese.empty()) return chinese;
    month = trim(month);
    year = trim(year);
    const KoreanWords* words = korean_words(month);
    if (words == nullptr || !number(year)) return {};
    return korean_month_year(*words, month, year);
}

std::string cjk_combat_title(std::string_view prefix, std::string_view place) {
    prefix = trim(prefix);
    place = trim(place);
    if (prefix.empty() || place.empty()) return {};
    const char32_t first = first_scalar(prefix);
    if (hangul(first)) return std::string(place) + " " + std::string(prefix);
    if (han_or_kana(first)) return std::string(place) + std::string(prefix);
    return {};
}

}  // namespace unicode_patch
