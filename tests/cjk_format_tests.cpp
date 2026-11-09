// Dates and combat titles in the translation's word order: the language comes from
// the words (月 or 월 in the month, Han, kana or Hangul in the title), the forms are
// those EU4DLL writes, and anything else keeps the game's text (empty).
#include "cjk_format.hpp"

#include <cstdio>
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

}  // namespace

int cjk_format_tests() {
    using unicode_patch::cjk_combat_title;
    using unicode_patch::cjk_date;
    using unicode_patch::cjk_month_year;

    check(cjk_date("11", utf8(u8"11月"), "1444") == utf8(u8"1444年11月11日"), "Chinese and Japanese date");
    check(cjk_date("8", utf8(u8"十月"), "1356") == utf8(u8"1356年十月8日"), "a month in characters");
    check(cjk_date("11", utf8(u8"11월"), "1444") == utf8(u8"1444년 11월 11일"), "Korean date");
    check(cjk_date("11", "November", "1444").empty(), "an English month keeps the game's date");
    check(cjk_date("XI", utf8(u8"11월"), "1444").empty(), "a day that is not a number");
    check(cjk_date("11", utf8(u8"월"), "1444").empty(), "a Korean month without its number");

    check(cjk_month_year(utf8(u8"4月 ,"), "1447") == utf8(u8"1447年4月"), "M, Y in Chinese");
    check(cjk_month_year(utf8(u8"4月 "), "1447") == utf8(u8"1447年4月"), "M Y in Chinese");
    check(cjk_month_year(utf8(u8"4월 ,"), "1447") == utf8(u8"1447년 4월"), "M, Y in Korean");
    check(cjk_month_year("April, ", "1447").empty(), "an English month keeps the game's text");
    check(cjk_month_year(utf8(u8"4月 "), "year").empty(), "a year that is not a number");

    check(cjk_combat_title(utf8(u8"之战 "), utf8(u8"巴黎")) == utf8(u8"巴黎之战"), "the original's 之战");
    check(cjk_combat_title(utf8(u8"之圍 "), utf8(u8"倫敦")) == utf8(u8"倫敦之圍"), "traditional 之圍");
    check(cjk_combat_title(utf8(u8"战役 "), utf8(u8"瓦尔纳")) == utf8(u8"瓦尔纳战役"), "52汉化组's battle");
    check(cjk_combat_title(utf8(u8"围攻 "), utf8(u8"君士坦丁堡")) == utf8(u8"君士坦丁堡围攻"), "52汉化组's siege");
    check(cjk_combat_title(utf8(u8"占领 "), utf8(u8"北京")) == utf8(u8"北京占领"), "an occupation, as EU4DLL");
    check(cjk_combat_title(utf8(u8"の戦い "), utf8(u8"京都")) == utf8(u8"京都の戦い"), "Japanese kana");
    check(cjk_combat_title(utf8(u8"전투 "), utf8(u8"바르나")) == utf8(u8"바르나 전투"), "Korean, with a space");
    check(cjk_combat_title("Battle of ", "Varna").empty(), "English keeps the game's order");
    check(cjk_combat_title(utf8(u8"战役 "), "").empty(), "no place");
    check(cjk_combat_title("", utf8(u8"北京")).empty(), "no prefix");
    return g_failures;
}
