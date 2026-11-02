"""Writes the characters of Mei's bundled font atlases as one UTF-8 line.

Usage: python -I tools/mei-charset.py <output.txt> [<localisation folder> ...]

The base is the common character sets of the four scripts: all of GB 2312 (simplified
Chinese), level 1 of Big5 and its symbols (traditional Chinese), the Hangul syllables and
symbols of KS X 1001 (Korean) and level 1 of JIS X 0208 with its symbols and kana
(Japanese). Each folder given (for example a translation mod's) adds every character above
U+00FF of its .yml files in UTF-8 and every glyph above U+00FF of its .fnt fonts: an EU4DLL
pack's fonts hold the characters of its escaped localisation.
"""
import sys
from pathlib import Path


def double_byte(codec, leads, trails):
    chars = set()
    for lead in leads:
        for trail in trails:
            try:
                chars.update(bytes([lead, trail]).decode(codec))
            except UnicodeDecodeError:
                pass
    return chars


BIG5_TRAILS = list(range(0x40, 0x7F)) + list(range(0xA1, 0xFF))
BASE = (double_byte("gb2312", range(0xA1, 0xF8), range(0xA1, 0xFF))
        | double_byte("big5", range(0xA1, 0xC7), BIG5_TRAILS)
        | double_byte("euc_kr", list(range(0xA1, 0xAD)) + list(range(0xB0, 0xC9)), range(0xA1, 0xFF))
        | double_byte("euc_jp", list(range(0xA1, 0xA9)) + list(range(0xB0, 0xD0)), range(0xA1, 0xFF)))


def folder_chars(folder: Path) -> set:
    chars = set()
    for path in folder.rglob("*.fnt"):
        for line in path.read_text(encoding="latin-1").splitlines():
            if line.startswith("char id="):
                code = int(line.split()[1].removeprefix("id="))
                if code > 0xFF and not 0xD800 <= code <= 0xDFFF:
                    chars.add(chr(code))
    for path in folder.rglob("*.yml"):
        try:
            text = path.read_bytes().decode("utf-8")
        except UnicodeDecodeError:
            continue
        if any("\x10" <= c <= "\x13" for c in text):
            continue  # EU4DLL escapes: the pack's fonts hold its characters
        chars.update(c for c in text if ord(c) > 0xFF)
    return chars


def main() -> None:
    chars = {c for c in BASE if ord(c) > 0xFF}
    for folder in sys.argv[2:]:
        extra = folder_chars(Path(folder)) - chars
        print(f"{folder}: {len(extra)} characters beyond the base", file=sys.stderr)
        chars |= extra
    chars = {c for c in chars if c.isprintable()}
    Path(sys.argv[1]).write_text("".join(sorted(chars)), encoding="utf-8")
    print(f"{len(chars)} characters above U+00FF", file=sys.stderr)


if __name__ == "__main__":
    main()
