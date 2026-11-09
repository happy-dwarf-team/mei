// Dates and combat titles in the word order of the translation's language. The game
// builds them in English order ("11 November 1444", "Battle of Varna"); a translation
// mod only changes the words. The language is read from the words themselves: a month
// that ends with 月 is Chinese or Japanese, one that ends with 월 Korean, and a title's
// prefix in Han or kana is Chinese or Japanese, one in Hangul Korean.
//
// The original (localized_format.cpp) does the Chinese dates and only the titles 之战
// and 之围; as EU4DLL does, Mei turns every Chinese or Japanese title round (战役, 围攻,
// 占领 of the current translation too) and adds Korean.
#pragma once

#include <string>
#include <string_view>

namespace unicode_patch {

// "1444年11月11日" or "1444년 11월 11일"; empty keeps the game's text.
std::string cjk_date(std::string_view day, std::string_view month, std::string_view year);
// "1444年11月" or "1444년 11월" from a month with a trailing " ," or " " and a year.
std::string cjk_month_year(std::string_view month, std::string_view year);
// "瓦尔纳战役" or "바르나 전투" from the title's prefix and place.
std::string cjk_combat_title(std::string_view prefix, std::string_view place);

}  // namespace unicode_patch
