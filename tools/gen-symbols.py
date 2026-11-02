"""Writes symbols.toml from the original's hook tables, so that every address is written once.

Usage: python -I tools/gen-symbols.py <eu4.exe> [upstream] > symbols.toml

It reads the original (the `upstream` submodule): the MinHook table and calls in src/plugin.cpp,
the stubs in src/*_hooks.asm and the continuation addresses they jump to, and the byte patches.
The bytes come from <eu4.exe> (EU4 1.37.5 Steam, SHA-256 9AD3EFE1...D771A); `anvil check . --exe`
then verifies them and the instruction boundaries.
"""
import re
import struct
import sys
from pathlib import Path

exe = Path(sys.argv[1]).read_bytes()
src = Path(sys.argv[2] if len(sys.argv) > 2 else "upstream")
plugin = (src / "src/plugin.cpp").read_text(encoding="utf-8")

pe = struct.unpack_from("<I", exe, 0x3C)[0]
count = struct.unpack_from("<H", exe, pe + 6)[0]
optional = struct.unpack_from("<H", exe, pe + 20)[0]
sections = [struct.unpack_from("<IIII", exe, pe + 24 + optional + 40 * i + 8) for i in range(count)]


def at(rva, n):
    for vsize, va, rsize, raw in sections:
        if va <= rva < va + max(vsize, rsize):
            return exe[raw + rva - va: raw + rva - va + n]
    raise ValueError(hex(rva))


def hexbytes(b):
    return " ".join(f"{x:02X}" for x in b)


# Function hooks are declared up to the first instruction boundary at or after 16 bytes
# (anvil check names the boundary when a declaration cuts an instruction).
FUNCTION_BYTES = {0x16FD650: 20, 0x1705180: 21, 0x1704AF0: 17, 0x159B470: 18, 0x16FC3D0: 20, 0x170CD10: 17,
                  0x170D1D0: 21, 0x170EBF0: 18, 0x15105B0: 20, 0x1594360: 18, 0xFD5B70: 20, 0x16D6640: 19,
                  0x1765850: 17, 0x1764940: 18, 0x15385A0: 20, 0x15361F0: 20, 0x1536060: 20, 0x1535EB0: 20,
                  0x1537210: 20, 0x1534250: 19, 0x15353F0: 17, 0x14DB940: 17, 0x1534100: 17, 0xB19590: 18,
                  0x15354E0: 20, 0x1536B80: 19, 0x1506D30: 20, 0x14C7890: 20, 0xE3D40: 18, 0x1539820: 19,
                  0x95110: 18}
# The hooks this plugin makes so far, with their C signatures.
SIGNATURES = {
    "find_country_name": "uint64_t (const char* name, uint64_t length, uint64_t start, const char* query, uint64_t query_length)",
    "find_province_distance": "int64_t (const void* name, const void* query)",
    "import_legacy_localization": "void (const void* key, const void* value, int version, int unused, const void* context)",
    "parse_utf8_yaml": "void (void* file, const void* input, const char* name, void* language, void* key, void* context)",
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
# Values of blocks, taken from their verified bytes: the original's address(0x...) for the same data.
VALUES = {
    "main_copy_block": ['values.copy_buffer = { at = 20, rip_relative = 24, type = "uint8_t" }'],
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

out = ["# Generated by tools/gen-symbols.py from the original's hook tables (upstream/, see",
       "# README.md for the commit) and eu4.exe 1.37.5 Steam. Do not edit by hand: change the",
       "# generator and run `anvil check . --exe <eu4.exe>` to write symbols.lock.",
       'build = "eu4-1.37.5-steam-win64"', ""]
continuations = {}
for rva, callback, trampoline in hooks:
    if callback in asm:
        stub, targets = asm[callback]
        conts = sorted((returns[g], g) for g in targets)
        later = [c for c in conts if c[0] > rva and c[0] - rva <= 96]
        name = callback.removesuffix("_hook") + "_block"
        out += [f"[symbol.{name}]", f"rva = 0x{rva:X}", 'memory = "code"']
        end = later[0][0] if later else None
        out.append(f'bytes = "{hexbytes(at(rva, (end - rva) if end else 5))}"')
        if end:
            out.append(f'resume = "{later[0][1].removeprefix("g_")}"')
        out += VALUES.get(name, [])
        others = [g.removeprefix("g_") for c, g in conts if c != end]
        note = f"{stub}.asm {callback}" + (f"; also continues at {', '.join(others)}" if others else "")
        out += [f'note = "{note}"', ""]
        for c, g in conts:
            continuations.setdefault(c, g.removeprefix("g_"))
    else:
        b = at(rva, FUNCTION_BYTES.get(rva, 16))
        out += [f"[symbol.{callback}]", f"rva = 0x{rva:X}", 'memory = "code"', f'bytes = "{hexbytes(b)}"']
        if callback in SIGNATURES:
            out.append(f'signature = "{SIGNATURES[callback]}"')
        out += [f'note = "function hook of the original{"" if trampoline else " that does not call the game function"} ({callback})"', ""]
for name, (signature, note) in CALLS.items():
    rva = int(re.search(rf"\b{name}=reinterpret_cast<[\w:]+>\(address\(0x([0-9a-f]+)\)\)", plugin).group(1), 16)
    out += [f"[symbol.{name}]", f"rva = 0x{rva:X}", 'memory = "code"', f'bytes = "{hexbytes(at(rva, FUNCTION_BYTES.get(rva, 16)))}"',
            f'signature = "{signature}"', f'note = "{note}"', ""]
for c, name in sorted(continuations.items()):
    out += [f"[symbol.{name}]", f"rva = 0x{c:X}", 'memory = "code"', f'bytes = "{hexbytes(at(c, 4))}"',
            'note = "where a replaced block continues"', ""]
for rva, before in patches:
    if rva in CONSTANTS:
        name, note = CONSTANTS[rva]
    else:
        target = (rva + 5 + struct.unpack("<i", before[1:5])[0]) & 0xFFFFFFFF
        prefix, what = CALL_TARGETS[target]
        name, note = f"{prefix}_call_{rva:x}", f"call 0x{target:X}, {what}; the original NOPs it"
    out += [f"[symbol.{name}]", f"rva = 0x{rva:X}", 'memory = "code"', f'bytes = "{hexbytes(at(rva, len(before)))}"',
            f'note = "{note}"', ""]
sys.stdout.write("\n".join(out))
