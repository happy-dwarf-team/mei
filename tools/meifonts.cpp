// Mei's bundled font atlases: the characters of a UTF-8 file, drawn from the given font
// files only (no system font), as the game's bitmap fonts (.fnt and .dds).
//
// Usage: meifonts <characters.txt> <output folder> --font <file> [--font <file> ...]
//                 <name>=<size>,<width>,<argb|bc3>,<white|black>[,mips] ...
//
// The glyph layout is the original's (upstream src/font_atlas_assets.cpp write_font_atlas):
// one glyph per DirectWrite layout, trimmed to its ink, packed in rows. Besides the file's
// characters every atlas holds U+0020-U+007E and U+00A0-U+00FF, and at 0x80-0x9F the
// Windows-1252 characters of those bytes, where the game's own fonts hold them: text that
// is not UTF-8 (names from scripts and saves) is drawn one byte per glyph.
// bc3 stores the alpha in 4x4 blocks (one byte a pixel instead of four), for the large map
// atlas; mips adds the mip chain the game's own Mapfont has. A character the font files
// lack is left out (and counted), so the game draws it as it draws any missing glyph.
#include "unicode_layout.hpp"
#include "unicode_text.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Spec {
    std::string name;
    int size = 0, width = 0;
    bool bc3 = false, black = false, mips = false;
};

struct Glyph {
    std::uint32_t id = 0;
    int x = 0, y = 0, width = 1, height = 1, x_offset = 0, y_offset = 0, advance = 0;
    std::vector<std::uint8_t> alpha;
};

// Windows-1252 at 0x80-0x9F (0 where the code page has no character).
constexpr std::array<std::uint32_t, 32> kCp1252 = {
    0x20AC, 0,      0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021, 0x02C6, 0x2030, 0x0160,
    0x2039, 0x0152, 0,      0x017D, 0,      0,      0x2018, 0x2019, 0x201C, 0x201D, 0x2022,
    0x2013, 0x2014, 0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0,      0x017E, 0x0178};

Spec parse_spec(const std::wstring& arg) {
    const auto eq = arg.find(L'=');
    if (eq == std::wstring::npos) throw std::invalid_argument("atlas spec must be name=size,width,format,colour");
    Spec spec;
    for (const wchar_t c : arg.substr(0, eq)) spec.name.push_back(static_cast<char>(c));
    std::vector<std::wstring> parts;
    std::wstring rest = arg.substr(eq + 1);
    for (std::size_t start = 0;;) {
        const auto comma = rest.find(L',', start);
        parts.push_back(rest.substr(start, comma - start));
        if (comma == std::wstring::npos) break;
        start = comma + 1;
    }
    if (parts.size() < 4) throw std::invalid_argument("atlas spec must be name=size,width,format,colour");
    spec.size = std::stoi(parts[0]);
    spec.width = std::stoi(parts[1]);
    spec.bc3 = parts[2] == L"bc3";
    spec.black = parts[3] == L"black";
    spec.mips = parts.size() > 4 && parts[4] == L"mips";
    if (spec.size <= 0 || spec.size > 512 || spec.width < 256 || spec.width > 8192 || (spec.width & (spec.width - 1)))
        throw std::invalid_argument("bad atlas size or width");
    return spec;
}

// The glyph of a character, or none when the font files lack it (DirectWrite would take it
// from a system font, which may not be bundled).
std::optional<Glyph> draw(std::uint32_t id, std::uint32_t scalar, int size,
                          const std::shared_ptr<const eu4unicode::TextFonts>& fonts,
                          const std::vector<std::string>& families) {
    const int base = static_cast<int>(std::round(size * 0.8));
    eu4unicode::TextLayout layout(eu4unicode::encode(scalar), static_cast<float>(size), 512, 512, L"Source Han Sans SC",
                                  fonts);
    for (const auto& run : layout.glyph_runs()) {
        const bool own = std::find(families.begin(), families.end(), run.font_family) != families.end();
        if (!own || std::count(run.glyphs.begin(), run.glyphs.end(), 0)) return std::nullopt;
    }
    const auto image = layout.rasterize();
    int left = static_cast<int>(image.width), top = static_cast<int>(image.height), right = -1, bottom = -1;
    for (std::uint32_t iy = 0; iy < image.height; ++iy)
        for (std::uint32_t ix = 0; ix < image.width; ++ix)
            if (image.pixels[(static_cast<std::size_t>(iy) * image.width + ix) * 4 + 3]) {
                left = (std::min)(left, static_cast<int>(ix));
                top = (std::min)(top, static_cast<int>(iy));
                right = (std::max)(right, static_cast<int>(ix));
                bottom = (std::max)(bottom, static_cast<int>(iy));
            }
    Glyph glyph;
    glyph.id = id;
    glyph.advance = static_cast<int>(std::ceil(layout.metrics().width));
    if (right >= left) {
        glyph.width = right - left + 1;
        glyph.height = bottom - top + 1;
        glyph.x_offset = left - 16;
        glyph.y_offset = base - static_cast<int>(std::round(image.baseline)) + top - 16;
        glyph.alpha.resize(static_cast<std::size_t>(glyph.width) * glyph.height);
        for (int iy = 0; iy < glyph.height; ++iy)
            for (int ix = 0; ix < glyph.width; ++ix)
                glyph.alpha[static_cast<std::size_t>(iy) * glyph.width + ix] =
                    image.pixels[(static_cast<std::size_t>(top + iy) * image.width + left + ix) * 4 + 3];
    } else {
        glyph.alpha = {0};
    }
    return glyph;
}

// One BC3 alpha block: the better of the 8-value and the 6-value (with exact 0 and 255) palettes.
void alpha_block(const std::uint8_t (&a)[16], std::uint8_t* out) {
    auto encode = [&](int a0, int a1, bool eight, std::uint64_t& bits) {
        int palette[8] = {a0, a1};
        if (eight) {
            for (int i = 1; i < 7; ++i) palette[i + 1] = ((7 - i) * a0 + i * a1 + 3) / 7;
        } else {
            for (int i = 1; i < 5; ++i) palette[i + 1] = ((5 - i) * a0 + i * a1 + 2) / 5;
            palette[6] = 0;
            palette[7] = 255;
        }
        int error = 0;
        bits = 0;
        for (int p = 0; p < 16; ++p) {
            int best = 0, best_error = 1 << 30;
            for (int i = 0; i < 8; ++i) {
                const int e = std::abs(palette[i] - a[p]);
                if (e < best_error) best_error = e, best = i;
            }
            error += best_error * best_error;
            bits |= static_cast<std::uint64_t>(best) << (3 * p);
        }
        return error;
    };
    int lo = 255, hi = 0, inner_lo = 255, inner_hi = 0;
    for (const auto v : a) {
        lo = (std::min)(lo, static_cast<int>(v));
        hi = (std::max)(hi, static_cast<int>(v));
        if (v != 0 && v != 255) inner_lo = (std::min)(inner_lo, static_cast<int>(v)), inner_hi = (std::max)(inner_hi, static_cast<int>(v));
    }
    std::uint64_t eight_bits = 0, six_bits = 0;
    // a0 > a1 selects the 8-value palette; equal end points take the 6-value one.
    const int eight_error = hi > lo ? encode(hi, lo, true, eight_bits) : 1 << 30;
    if (inner_lo > inner_hi) inner_lo = inner_hi = lo == 255 ? 255 : 0;
    const int six_error = encode(inner_lo, inner_hi, false, six_bits);
    const bool eight = eight_error < six_error;
    out[0] = static_cast<std::uint8_t>(eight ? hi : inner_lo);
    out[1] = static_cast<std::uint8_t>(eight ? lo : inner_hi);
    const auto bits = eight ? eight_bits : six_bits;
    for (int i = 0; i < 6; ++i) out[2 + i] = static_cast<std::uint8_t>(bits >> (8 * i));
}

void write_dds(const std::filesystem::path& path, const Spec& spec, int height, std::vector<std::uint8_t> alpha) {
    std::vector<std::vector<std::uint8_t>> levels{alpha};
    int w = spec.width, h = height;
    while (spec.mips && (w > 1 || h > 1)) {
        const int nw = (std::max)(1, w / 2), nh = (std::max)(1, h / 2);
        std::vector<std::uint8_t> next(static_cast<std::size_t>(nw) * nh);
        const auto& prev = levels.back();
        for (int y = 0; y < nh; ++y)
            for (int x = 0; x < nw; ++x) {
                int sum = 0, n = 0;
                for (int dy = 0; dy < 2; ++dy)
                    for (int dx = 0; dx < 2; ++dx) {
                        const int sx = (std::min)(w - 1, 2 * x + dx), sy = (std::min)(h - 1, 2 * y + dy);
                        sum += prev[static_cast<std::size_t>(sy) * w + sx], ++n;
                    }
                next[static_cast<std::size_t>(y) * nw + x] = static_cast<std::uint8_t>((sum + n / 2) / n);
            }
        levels.push_back(std::move(next));
        w = nw, h = nh;
    }
    std::array<std::uint32_t, 32> header{};
    header[0] = 0x20534444;  // "DDS "
    header[1] = 124;
    header[3] = static_cast<std::uint32_t>(height);
    header[4] = static_cast<std::uint32_t>(spec.width);
    header[7] = static_cast<std::uint32_t>(levels.size());
    header[19] = 32;
    header[27] = 0x1000 | (levels.size() > 1 ? 0x400008 : 0);  // texture, mipmap, complex
    if (spec.bc3) {
        header[2] = 0x81007 | (levels.size() > 1 ? 0x20000 : 0);  // linear size, mip count
        header[5] = static_cast<std::uint32_t>(((spec.width + 3) / 4) * ((height + 3) / 4) * 16);
        header[20] = 0x4;  // four CC
        header[21] = 0x35545844;  // "DXT5"
    } else {
        header[2] = 0x100f | (levels.size() > 1 ? 0x20000 : 0);
        header[5] = static_cast<std::uint32_t>(spec.width * 4);
        header[20] = 0x41;  // RGB with alpha
        header[22] = 32;
        header[23] = 0x00ff0000, header[24] = 0x0000ff00, header[25] = 0x000000ff, header[26] = 0xff000000;
    }
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(header.data()), sizeof(header));
    // The UI masks are white; the map shader takes the atlas colour as the label's.
    const std::uint8_t colour = spec.black ? 0 : 255;
    w = spec.width, h = height;
    for (const auto& level : levels) {
        if (spec.bc3) {
            for (int by = 0; by < (h + 3) / 4; ++by)
                for (int bx = 0; bx < (w + 3) / 4; ++bx) {
                    std::uint8_t block[16]{};
                    for (int p = 0; p < 16; ++p) {
                        const int x = (std::min)(w - 1, bx * 4 + p % 4), y = (std::min)(h - 1, by * 4 + p / 4);
                        block[p] = level[static_cast<std::size_t>(y) * w + x];
                    }
                    std::uint8_t out[16]{};
                    alpha_block(block, out);
                    const std::uint16_t c = spec.black ? 0x0000 : 0xFFFF;
                    out[8] = out[10] = static_cast<std::uint8_t>(c);
                    out[9] = out[11] = static_cast<std::uint8_t>(c >> 8);
                    file.write(reinterpret_cast<const char*>(out), sizeof(out));
                }
        } else {
            std::vector<std::uint8_t> row(static_cast<std::size_t>(w) * 4);
            for (int y = 0; y < h; ++y) {
                for (int x = 0; x < w; ++x) {
                    row[x * 4] = row[x * 4 + 1] = row[x * 4 + 2] = colour;
                    row[x * 4 + 3] = level[static_cast<std::size_t>(y) * w + x];
                }
                file.write(reinterpret_cast<const char*>(row.data()), static_cast<std::streamsize>(row.size()));
            }
        }
        w = (std::max)(1, w / 2), h = (std::max)(1, h / 2);
    }
    if (!file) throw std::runtime_error("cannot write " + path.string());
}

std::size_t write_atlas(const Spec& spec, const std::map<std::uint32_t, std::uint32_t>& ids,
                        const std::shared_ptr<const eu4unicode::TextFonts>& fonts, const std::filesystem::path& out) {
    const auto families = fonts->families();
    std::size_t missing = 0;
    std::vector<Glyph> glyphs;
    int x = 1, y = 1, row = 0;
    for (const auto& [id, scalar] : ids) {
        auto drawn = draw(id, scalar, spec.size, fonts, families);
        if (!drawn) {
            ++missing;
            continue;
        }
        auto& glyph = *drawn;
        if (glyph.width + 2 > spec.width) throw std::runtime_error("a glyph is wider than the atlas");
        if (x + glyph.width + 1 > spec.width) x = 1, y += row + 1, row = 0;
        glyph.x = x, glyph.y = y;
        x += glyph.width + 1;
        row = (std::max)(row, glyph.height);
        glyphs.push_back(std::move(glyph));
    }
    int height = 1;
    while (height < y + row + 1) height *= 2;
    if (height > 8192) throw std::runtime_error(spec.name + ": the characters do not fit in 8192 rows");
    std::vector<std::uint8_t> alpha(static_cast<std::size_t>(spec.width) * height);
    for (const auto& g : glyphs)
        for (int iy = 0; iy < g.height; ++iy)
            for (int ix = 0; ix < g.width; ++ix)
                alpha[static_cast<std::size_t>(g.y + iy) * spec.width + g.x + ix] =
                    g.alpha[static_cast<std::size_t>(iy) * g.width + ix];
    write_dds(out / (spec.name + ".dds"), spec, height, std::move(alpha));
    std::ofstream fnt(out / (spec.name + ".fnt"));
    const int base = static_cast<int>(std::round(spec.size * 0.8));
    // Not the font's own name: the OFL reserves "Source" for Adobe's fonts.
    fnt << "info face=\"Mei CJK\" size=" << spec.size
        << " bold=0 italic=0 charset=\"\" stretchH=100 smooth=1 aa=1 padding=0,0,0,0 spacing=1,1\n";
    fnt << "common lineHeight=" << spec.size << " base=" << base << " scaleW=" << spec.width << " scaleH=" << height
        << " pages=1\n";
    for (const auto& g : glyphs)
        fnt << "char id=" << g.id << " x=" << g.x << " y=" << g.y << " width=" << g.width << " height=" << g.height
            << " xoffset=" << g.x_offset << " yoffset=" << g.y_offset << " xadvance=" << g.advance << " page=0\n";
    if (!fnt) throw std::runtime_error("cannot write " + spec.name + ".fnt");
    std::cout << spec.name << ": " << glyphs.size() << " glyphs, " << spec.width << 'x' << height
              << (spec.bc3 ? " BC3" : " A8R8G8B8") << (spec.mips ? " with mips" : "") << ", " << missing
              << " characters not in the fonts\n";
    return missing;
}

}  // namespace

int wmain(int argc, wchar_t** argv) {
    try {
        if (argc < 4) throw std::invalid_argument("usage: meifonts <characters.txt> <output> --font <file> ... <atlas spec> ...");
        std::vector<std::filesystem::path> files;
        std::vector<Spec> specs;
        for (int i = 3; i < argc; ++i) {
            const std::wstring arg = argv[i];
            if (arg == L"--font" && i + 1 < argc) files.emplace_back(argv[++i]);
            else specs.push_back(parse_spec(arg));
        }
        if (files.empty() || specs.empty()) throw std::invalid_argument("need --font and at least one atlas");
        const auto fonts = std::make_shared<eu4unicode::TextFonts>(files);
        std::ifstream source(argv[1], std::ios::binary);
        const std::string content((std::istreambuf_iterator<char>(source)), std::istreambuf_iterator<char>());
        if (!source.eof() && !source) throw std::runtime_error("cannot read the characters");
        if (!eu4unicode::valid_utf8(content)) throw std::invalid_argument("the characters must be UTF-8");
        // Glyph id -> the character drawn for it.
        std::map<std::uint32_t, std::uint32_t> ids;
        for (std::uint32_t c = 0x20; c <= 0xFF; ++c)
            if (c < 0x7F || c >= 0xA0) ids[c] = c;
        for (std::uint32_t b = 0x80; b <= 0x9F; ++b)
            if (kCp1252[b - 0x80]) ids[b] = kCp1252[b - 0x80], ids[kCp1252[b - 0x80]] = kCp1252[b - 0x80];
        for (auto rest = std::string_view(content); !rest.empty();) {
            const auto scalar = eu4unicode::decode(rest);
            if (scalar.value > 0xFF && scalar.value != 0xFEFF && !(scalar.value >= 0xD800 && scalar.value <= 0xDFFF))
                ids[scalar.value] = scalar.value;
            rest.remove_prefix(scalar.bytes);
        }
        const std::filesystem::path out(argv[2]);
        std::filesystem::create_directories(out);
        for (const auto& spec : specs) write_atlas(spec, ids, fonts, out);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return 2;
    }
}
