"""Writes symbols.toml from the original's hook tables, so that every address is written once.

Usage: python -I tools/gen-symbols.py <eu4.exe> [upstream] [--gog <gog eu4.exe> [map.json]] > symbols.toml

It reads the original (the `upstream` submodule): the MinHook table and calls in src/plugin.cpp,
the stubs in src/*_hooks.asm and the continuation addresses they jump to, and the byte patches.
The bytes come from <eu4.exe> (EU4 1.37.5 Steam, SHA-256 9AD3EFE1...D771A); `anvil check . --exe`
then verifies them and the instruction boundaries.

With --gog, the file lists the GOG build 1.37.5.2 too (Anvil design 5.2): each symbol's GOG address
comes from tools/gog-1.37.5.2.json (the lab's port, names from the GOG eu4.pdb), its bytes from the
GOG eu4.exe (SHA-256 D29CF745...155319), shared when they equal Steam's. A symbol the map gives
null is `absent` on GOG (Anvil 1.9).
"""
import json
import re
import struct
import sys
from pathlib import Path

STEAM, GOG = "eu4-1.37.5-steam-win64", "eu4-1.37.5.2-gog-win64"
args = sys.argv[1:]
gog_exe, gog_map = None, None
if "--gog" in args:
    i = args.index("--gog")
    gog_exe = Path(args[i + 1]).read_bytes()
    rest = args[i + 2:]
    map_path = Path(rest[0]) if rest and rest[0].endswith(".json") else Path(__file__).with_name("gog-1.37.5.2.json")
    gog_map = {k: (None if v is None else int(v, 16)) for k, v in json.loads(map_path.read_text(encoding="utf-8"))["symbols"].items()}
    args = args[:i] + rest[1 if rest and rest[0].endswith(".json") else 0:]
exe = Path(args[0]).read_bytes()
src = Path(args[1] if len(args) > 1 else "upstream")
plugin = (src / "src/plugin.cpp").read_text(encoding="utf-8")


def sections_of(image):
    pe = struct.unpack_from("<I", image, 0x3C)[0]
    count = struct.unpack_from("<H", image, pe + 6)[0]
    optional = struct.unpack_from("<H", image, pe + 20)[0]
    return [struct.unpack_from("<IIII", image, pe + 24 + optional + 40 * i + 8) for i in range(count)]


sections = sections_of(exe)
gog_sections = sections_of(gog_exe) if gog_exe else []


def at(rva, n, image=None, secs=None):
    image, secs = (exe, sections) if image is None else (image, secs)
    for vsize, va, rsize, raw in secs:
        if va <= rva < va + max(vsize, rsize):
            return image[raw + rva - va: raw + rva - va + n]
    raise ValueError(hex(rva))


def where(name, rva, n):
    """The keys that place a symbol: rva and, with n, its n bytes; for both builds with --gog."""
    if gog_map is None:
        return [f"rva = 0x{rva:X}"] + ([f'bytes = "{hexbytes(at(rva, n))}"'] if n else [])
    if name not in gog_map:
        raise SystemExit(f"{name}: not in the GOG map")
    other = gog_map[name]
    steam = at(rva, n) if n else None
    if other is None:
        return [f"rva = 0x{rva:X}"] + ([f'bytes = "{hexbytes(steam)}"'] if n else []) + [f'absent = ["{GOG}"]']
    if not n:
        return [f'build."{STEAM}" = {{ rva = 0x{rva:X} }}', f'build."{GOG}" = {{ rva = 0x{other:X} }}']
    gog = at(other, n, gog_exe, gog_sections)
    if gog == steam:
        return [f'bytes = "{hexbytes(steam)}"', f'build."{STEAM}" = {{ rva = 0x{rva:X} }}',
                f'build."{GOG}" = {{ rva = 0x{other:X} }}']
    return [f'build."{STEAM}" = {{ rva = 0x{rva:X}, bytes = "{hexbytes(steam)}" }}',
            f'build."{GOG}" = {{ rva = 0x{other:X}, bytes = "{hexbytes(gog)}" }}']


def hexbytes(b):
    return " ".join(f"{x:02X}" for x in b)


# Function hooks are declared up to the first instruction boundary at or after 16 bytes
# (anvil check names the boundary when a declaration cuts an instruction).
FUNCTION_BYTES = {0x16FD650: 20, 0x1705180: 21, 0x1704AF0: 17, 0x159B470: 18, 0x16FC3D0: 20, 0x170CD10: 17,
                  0x170D1D0: 21, 0x170EBF0: 18, 0x15105B0: 20, 0x1594360: 18, 0xFD5B70: 20, 0x16D6640: 19,
                  0x1765850: 17, 0x1764940: 18, 0x15385A0: 20, 0x15361F0: 20, 0x1536060: 20, 0x1535EB0: 20,
                  0x1537210: 20, 0x1534250: 19, 0x15353F0: 17, 0x14DB940: 17, 0x1534100: 17, 0xB19590: 18,
                  0x15354E0: 20, 0x1536B80: 19, 0x1506D30: 20, 0x14C7890: 20, 0xE3D40: 18, 0x1539820: 19, 0x1536F50: 20, 0x95660: 17,
                  0x95110: 18, 0x19FAD70: 19}
# The hooks this plugin makes so far, with their C signatures.
SIGNATURES = {
    "find_country_name": "uint64_t (const char* name, uint64_t length, uint64_t start, const char* query, uint64_t query_length)",
    "find_province_distance": "int64_t (const void* name, const void* query)",
    "import_legacy_localization": "void (const void* key, const void* value, int version, int unused, const void* context)",
    "parse_utf8_yaml": "void (void* file, const void* input, const char* name, void* language, void* key, void* context)",
    "destroy_font_table": "void (void* table)",
    "consume_editor_commit": "bool (void* callback, const char* character)",
    "insert_editor_commit": "void (void* widget, const void* text)",
    "editor_key": "bool (void* widget, const void* event)",
    "editor_left": "void (void* widget)",
    "editor_right": "void (void* widget)",
    "assign_editor_prefix": "void* (void* target, const char* text, uint64_t size)",
    "load_font_atlas": "void (void* font)",
    "construct_script_file": "void* (void* lexer, void* path)",
    "construct_script_file_mode": "void* (void* lexer, void* path, int mode)",
    "construct_script_stream": "void* (void* lexer, void* input, bool own_input)",
    "read_legacy_script_token": "int (void* lexer)",
    "read_legacy_binary_token": "int (void* lexer)",
}
# The game's functions this plugin calls, with their C signatures; the address is the
# original's `address(0x...)` for the same name in src/plugin.cpp.
CALLS = {
    "register_imported_text": ("void (void* collection, const char* key, const char* value, int line, int version, bool replace)",
                               "the game's registration of one localisation value, which import_legacy_localization calls"),
    "repeat_text": ("char* (void* target, uint64_t count, uint8_t character)",
                    "the game's std::string assign of count copies of a character, which the copy blocks call"),
    "append_text": ("void* (void* target, const char* text, uint64_t size)",
                    "the game's std::string append, which button_icon_copy_block calls"),
}
# Game functions and data the original reaches as image+0x... inside a function body
# (plugin.cpp consume_editor_commit), with their C signatures or types.
INLINE = {
    "find_element": (0x14DB3C0, 'signature = "void* (void* set, const void* key)"',
                     "the game's set lookup that consume_editor_commit calls for its seven-character restriction"),
    "string_destroy": (0x95660, 'signature = "void (void* target)"',
                       "the game's std::string destructor (native_editor_text.cpp)"),
    "editor_move_caret": (0x1536F50, 'signature = "void (void* widget, uint32_t offset)"',
                          "the text field's caret move to a byte offset (native_editor_text.cpp)"),
    "resolve_resource": (0x19FAD70, 'signature = "const void* (const char* path)"',
                         "the game's resource resolver: the first mounted source of a path (the original's native_font_source)"),
    "editor_restricted_characters": (0x1D91328, 'type = "char"',
                                     "the seven characters a restricted text field refuses (consume_editor_commit)"),
}
# Calls the original recognises by where they return on Steam (native_search.cpp, and plugin.cpp
# for text input): their call instructions, so that the plugin finds them on every build.
CALL_SITES = {
    0xEFC38F: ("country_find_call_efc38f", 5, "lobby search: find_country_name on a country's name"),
    0x1141FE6: ("country_find_call_1141fe6", 5, "goto box: find_country_name on a name"),
    0x11420DD: ("country_find_call_11420dd", 5, "goto box: find_country_name on a second name"),
    0x114218D: ("province_distance_call_114218d", 5, "goto box: find_province_distance on a name"),
    0x11421AD: ("province_distance_call_11421ad", 5, "goto box: find_province_distance on a second name"),
    0x14E9CB3: ("queued_character_call_14e9cb3", 3, "the input queue's type-2 consumer: consume_editor_commit through the callback"),
    0x153560F: ("commit_insert_call_153560f", 6, "consume_editor_commit's insertion: insert_editor_commit through the widget"),
}
# Values of blocks, taken from their verified bytes: the original's address(0x...) for the same data.
VALUES = {
    "main_copy_block": ['values.copy_buffer = { at = 20, rip_relative = 24, type = "uint8_t" }'],
    # call rel32 to the game's operator new (the original's g_engine_new)
    "font_allocate_block": ['values.engine_new = { at = 6, rip_relative = 10 }'],
}
# What the 26 byte patches change (Ghidra, EU4 engine lab docs/anvil-unicode-patch-research.md).
CALL_TARGETS = {0x1705180: ("save_transliterate", "CP1252 transliteration of a save name"),
                0x15503F0: ("search_lower", "per-byte tolower of a search key"),
                0x1704D70: ("search_unaccent", "Latin-1 accent stripping of a search key")}
CONSTANTS = {0x1595C88: ("font_codepoint_limit", "imm32 of cmp edi, 0xFF in the font loader; the original makes it 0x10FFFF"),
             0x16C2CBA: ("texture_file_limit", "imm32 of cmp r14d, 0x1000000 before PHYSFS_read; the original makes it 0x4000000"),
             0x1764E56: ("sdl_ime_ui_less", "lea r14,[rbx+1568h] in the SDL IME set-up; the original jumps past the UI-less TSF manager")}

asm = {}
for f in sorted(src.glob("src/*_hooks.asm")):
    for m in re.finditer(r"^(\w+) PROC\b(.*?)^\1 ENDP", f.read_text(encoding="utf-8", errors="replace"), re.S | re.M):
        asm[m.group(1)] = (f.stem, sorted(set(re.findall(r"jmp\s+qword ptr \[(g_\w+)\]", m.group(2)))))
returns = {m.group(1): int(m.group(2), 16) for m in re.finditer(r"\b(g_\w+)\s*=\s*address\(0x([0-9a-f]+)\)", plugin)}

hooks = []
table = re.search(r"const Hook hooks\[\]=\{(.*?)\};", plugin, re.S).group(1)
for m in re.finditer(r"\{0x([0-9a-f]+),reinterpret_cast<void\*>\(([\w:]+)\)\}", table):
    hooks.append((int(m.group(1), 16), m.group(2).split("::")[-1], False))
for m in re.finditer(r"MH_CreateHook\(image\+0x([0-9a-f]+),reinterpret_cast<void\*>\(([\w:]+)\),\s*(nullptr|reinterpret_cast)", plugin):
    hooks.append((int(m.group(1), 16), m.group(2).split("::")[-1], m.group(3) != "nullptr"))
patches = []
consts = re.search(r"const DataPatch constants\[\]=\{(.*?)\};", plugin, re.S).group(1)
for m in re.finditer(r"\{0x([0-9a-f]+),bytes\(\"([0-9a-f]+)\"\)", consts):
    patches.append((int(m.group(1), 16), bytes.fromhex(m.group(2))))

if gog_map:
    out = ["# Generated by tools/gen-symbols.py from the original's hook tables (upstream/, see",
           "# README.md for the commit), eu4.exe 1.37.5 Steam and, for the GOG build 1.37.5.2,",
           "# tools/gog-1.37.5.2.json. Do not edit by hand: change the generator or the map and run",
           "# `anvil check . --exe <eu4.exe>` once per build to write symbols.lock.",
           f'builds = ["{STEAM}", "{GOG}"]', ""]
else:
    out = ["# Generated by tools/gen-symbols.py from the original's hook tables (upstream/, see",
           "# README.md for the commit) and eu4.exe 1.37.5 Steam. Do not edit by hand: change the",
           "# generator and run `anvil check . --exe <eu4.exe>` to write symbols.lock.",
           f'build = "{STEAM}"', ""]
continuations = {}
for rva, callback, trampoline in hooks:
    if callback in asm:
        stub, targets = asm[callback]
        conts = sorted((returns[g], g) for g in targets)
        later = [c for c in conts if c[0] > rva and c[0] - rva <= 96]
        name = callback.removesuffix("_hook") + "_block"
        end = later[0][0] if later else None
        out += [f"[symbol.{name}]", *where(name, rva, (end - rva) if end else 5), 'memory = "code"']
        if end:
            out.append(f'resume = "{later[0][1].removeprefix("g_")}"')
        out += VALUES.get(name, [])
        others = [g.removeprefix("g_") for c, g in conts if c != end]
        note = f"{stub}.asm {callback}" + (f"; also continues at {', '.join(others)}" if others else "")
        out += [f'note = "{note}"', ""]
        for c, g in conts:
            continuations.setdefault(c, g.removeprefix("g_"))
    else:
        out += [f"[symbol.{callback}]", *where(callback, rva, FUNCTION_BYTES.get(rva, 16)), 'memory = "code"']
        if callback in SIGNATURES:
            out.append(f'signature = "{SIGNATURES[callback]}"')
        out += [f'note = "function hook of the original{"" if trampoline else " that does not call the game function"} ({callback})"', ""]
for name, (signature, note) in CALLS.items():
    rva = int(re.search(rf"\b{name}=reinterpret_cast<[\w:]+>\(address\(0x([0-9a-f]+)\)\)", plugin).group(1), 16)
    out += [f"[symbol.{name}]", *where(name, rva, FUNCTION_BYTES.get(rva, 16)), 'memory = "code"',
            f'signature = "{signature}"', f'note = "{note}"', ""]
for name, (rva, kind, note) in INLINE.items():
    if kind.startswith("signature"):
        out += [f"[symbol.{name}]", *where(name, rva, FUNCTION_BYTES.get(rva, 16)), 'memory = "code"', kind,
                f'note = "{note}"', ""]
    else:
        out += [f"[symbol.{name}]", *where(name, rva, 0), 'memory = "read"', kind, f'note = "{note}"', ""]
for rva, (name, size, note) in CALL_SITES.items():
    out += [f"[symbol.{name}]", *where(name, rva, size), 'memory = "code"',
            f'note = "{note}; the plugin maps where it returns to the Steam address the original checks"', ""]
for c, name in sorted(continuations.items()):
    out += [f"[symbol.{name}]", *where(name, c, 4), 'memory = "code"', 'note = "where a replaced block continues"', ""]
for rva, before in patches:
    if rva in CONSTANTS:
        name, note = CONSTANTS[rva]
    else:
        target = (rva + 5 + struct.unpack("<i", before[1:5])[0]) & 0xFFFFFFFF
        prefix, what = CALL_TARGETS[target]
        name, note = f"{prefix}_call_{rva:x}", f"call 0x{target:X}, {what}; the original NOPs it"
    out += [f"[symbol.{name}]", *where(name, rva, len(before)), 'memory = "code"', f'note = "{note}"', ""]
sys.stdout.write("\n".join(out))
