"""Lays out fontpack's atlases as a mod's fonts under the game's font names.

Usage: python -I tools/make-font-mod.py <fontpack output> <mod folder>

fontpack writes zh-hans-14, -16, -18, -24 and -map (.fnt and .dds). Each of the game's UI fonts
gets the atlas of its size, as the original maps them (upstream src/font_assets.cpp), as
<mod>/gfx/fonts/<name>.fnt, .dds and .tga (the game has both texture forms for some fonts).
Mapfont and tahoma_60 are left out: the map labels are not ported yet (README.md "State").
"""
import shutil
import struct
import sys
from pathlib import Path

SIZES = {"Arial12": 14, "Arial12_bold": 14, "garamond_12": 14, "garamond_14": 14, "garamond_14_bold": 14,
         "garamond_16": 16, "garamond_16_bold": 16, "garamond_24": 24, "standard": 14, "standard_18": 14,
         "standard_22": 14, "tahoma_20_bold": 14, "vic_18": 16, "vic_18_grey": 16, "vic_18s": 16, "vic_22": 18,
         "vic_22_bl": 18, "vic_22s": 18, "vic_29": 24, "vic_29s": 24, "vic_36": 24, "vic_36s": 24}


def tga_from_dds(dds: bytes) -> bytes:
    """fontpack's DDS is uncompressed A8R8G8B8 (bytes B, G, R, A), as a 32-bit TGA stores them."""
    height, width = struct.unpack_from("<II", dds, 12)
    pixels = dds[128:128 + width * height * 4]
    return struct.pack("<BBBHHBHHHHBB", 0, 0, 2, 0, 0, 0, 0, 0, width, height, 32, 0x28) + pixels  # top-left origin


def main() -> None:
    atlas, mod = Path(sys.argv[1]), Path(sys.argv[2])
    fonts = mod / "gfx" / "fonts"
    fonts.mkdir(parents=True, exist_ok=True)
    for name, size in SIZES.items():
        source = atlas / f"zh-hans-{size}"
        shutil.copyfile(source.with_suffix(".fnt"), fonts / f"{name}.fnt")
        dds = source.with_suffix(".dds").read_bytes()
        (fonts / f"{name}.dds").write_bytes(dds)
        (fonts / f"{name}.tga").write_bytes(tga_from_dds(dds))
    print(f"{len(SIZES)} fonts in {fonts}")


if __name__ == "__main__":
    main()
