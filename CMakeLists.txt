cmake_minimum_required(VERSION 3.25)
project(mei LANGUAGES CXX)

include(FetchContent)

# The Anvil SDK: the AnvilPlugin CMake package (anvil_add_plugin) and anvil.exe,
# from the SDK archive of an Anvil release. URL_HASH is the archive's line in
# the release's SHA256SUMS.txt.
FetchContent_Declare(anvil_sdk
    URL https://github.com/happy-dwarf-team/anvil/releases/download/v1.8.0/anvil-sdk-windows-x64.zip
    URL_HASH SHA256=1c407a71acd211797dbb2f70cba3c16efbb50c767735dcd1ef3acf7bdc98b765)
FetchContent_MakeAvailable(anvil_sdk)

# The original's text logic, built from the upstream/ submodule as its own
# cmake/shared-core.cmake and cmake/windows.cmake build it (same sources, the
# static CRT the plugin uses). Nothing of it is copied into this repository.
set(UPSTREAM "${CMAKE_CURRENT_SOURCE_DIR}/upstream")
if(NOT EXISTS "${UPSTREAM}/src/unicode_search.cpp" OR NOT EXISTS "${UPSTREAM}/vendor/utfcpp/source/utf8.h")
    message(FATAL_ERROR "upstream/ is missing: git submodule update --init upstream && "
            "git -C upstream submodule update --init vendor/utfcpp")
endif()
find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_library(unicode_text STATIC
    ${UPSTREAM}/src/unicode_text.cpp ${UPSTREAM}/src/native_text_event.cpp ${UPSTREAM}/src/yaml_input.cpp)
target_include_directories(unicode_text PUBLIC ${UPSTREAM}/include PRIVATE ${UPSTREAM}/vendor/utfcpp/source)
add_library(unicode_services STATIC ${UPSTREAM}/src/unicode_services.cpp)
# icu.dll of Windows 10 1903 and later, as in the original.
target_link_libraries(unicode_services PUBLIC unicode_text icu)

set(PINYIN_DATA "${CMAKE_CURRENT_BINARY_DIR}/generated/pinyin_data.inc")
add_custom_command(OUTPUT "${PINYIN_DATA}"
    COMMAND "${Python3_EXECUTABLE}" -I "${UPSTREAM}/tools/generate-pinyin-data.py"
        "${UPSTREAM}/vendor/phrase-pinyin-data" "${PINYIN_DATA}"
    DEPENDS "${UPSTREAM}/tools/generate-pinyin-data.py" "${UPSTREAM}/vendor/phrase-pinyin-data/pinyin.txt"
    VERBATIM)
add_library(unicode_search STATIC
    ${UPSTREAM}/src/unicode_search.cpp ${UPSTREAM}/src/unicode_search_fuzzy.cpp ${UPSTREAM}/src/unicode_pinyin.cpp
    ${UPSTREAM}/src/search_resources.cpp ${UPSTREAM}/src/native_search.cpp "${PINYIN_DATA}")
target_include_directories(unicode_search PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/generated" ${UPSTREAM}/vendor/utfcpp/source)
target_link_libraries(unicode_search PUBLIC unicode_services)
set_source_files_properties(${UPSTREAM}/src/unicode_pinyin.cpp PROPERTIES COMPILE_OPTIONS /bigobj)

add_library(formatted_text STATIC ${UPSTREAM}/src/formatted_text.cpp ${UPSTREAM}/src/formatted_text_cache.cpp)
target_link_libraries(formatted_text PUBLIC unicode_services)
add_library(unicode_editor STATIC ${UPSTREAM}/src/unicode_editor.cpp ${UPSTREAM}/src/editor_document.cpp
    ${UPSTREAM}/src/native_editor_text.cpp)
target_link_libraries(unicode_editor PUBLIC unicode_services)
add_library(glyph_registry STATIC ${UPSTREAM}/src/glyph_registry.cpp)
target_include_directories(glyph_registry PUBLIC ${UPSTREAM}/include)
# The original's fontpack tool: font atlases (.fnt and .dds) of the characters of a
# UTF-8 file, drawn by DirectWrite from the system's fonts (tools/make-font-mod.py).
add_library(font_atlas_assets STATIC ${UPSTREAM}/src/unicode_layout.cpp ${UPSTREAM}/src/font_atlas_assets.cpp)
target_link_libraries(font_atlas_assets PUBLIC unicode_services dwrite d2d1 windowscodecs ole32)
target_compile_definitions(font_atlas_assets PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
add_executable(fontpack ${UPSTREAM}/tools/fontpack.cpp)
target_link_libraries(fontpack PRIVATE font_atlas_assets)
# Not /W4: unicode_layout.cpp hides a parameter (C4457), which is the original's to change.
set_target_properties(font_atlas_assets fontpack PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_compile_options(font_atlas_assets PRIVATE /utf-8 /EHsc)
target_compile_options(fontpack PRIVATE /utf-8 /EHsc)
# Mei's bundled atlases (fonts/README.md): the same layout from the pinned open font only.
add_executable(meifonts tools/meifonts.cpp)
target_link_libraries(meifonts PRIVATE font_atlas_assets)
set_target_properties(meifonts PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
target_compile_options(meifonts PRIVATE /W4 /utf-8 /EHsc)

add_library(native_script_bom STATIC ${UPSTREAM}/src/native_script_bom.cpp)
target_include_directories(native_script_bom PUBLIC ${UPSTREAM}/include PRIVATE ${UPSTREAM}/vendor/utfcpp/source)
add_library(legacy_text STATIC ${UPSTREAM}/src/legacy_text.cpp ${UPSTREAM}/src/native_legacy_import.cpp)
target_link_libraries(legacy_text PUBLIC unicode_text native_script_bom)

# C++17 as in the original: its u8"" literals are char there, char8_t in C++20.
foreach(target unicode_text unicode_services unicode_search formatted_text unicode_editor glyph_registry native_script_bom
               legacy_text)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    target_compile_options(${target} PRIVATE /W4 /utf-8 /EHsc)
endforeach()

# Reads plugin.toml, runs `anvil gen` on symbols.toml and settings.toml, builds
# the DLL and lays out build/plugin/<id>/. Adds mei-check and, with
# -DEU4_DIR=<game folder>, mei-deploy.
# The plugin's glue to the original's search, shared with port_tests.
add_library(port_search STATIC src/game_text.cpp)
target_include_directories(port_search PUBLIC src)
target_compile_features(port_search PUBLIC cxx_std_20)
target_link_libraries(port_search PUBLIC unicode_search)
target_compile_options(port_search PRIVATE /W4 /utf-8 /EHsc)
# The plugin's glue to the original's localisation import, shared with port_tests.
add_library(port_localisation STATIC src/localisation.cpp)
target_include_directories(port_localisation PUBLIC src)
target_compile_features(port_localisation PUBLIC cxx_std_20)
target_link_libraries(port_localisation PUBLIC legacy_text)
target_compile_options(port_localisation PRIVATE /W4 /utf-8 /EHsc)

# The decoding of the per-glyph text blocks, shared with port_tests.
add_library(port_text_drawing STATIC src/text_drawing.cpp)
target_include_directories(port_text_drawing PUBLIC src)
target_compile_features(port_text_drawing PUBLIC cxx_std_20)
target_link_libraries(port_text_drawing PUBLIC formatted_text)
target_compile_options(port_text_drawing PRIVATE /W4 /utf-8 /EHsc)

anvil_add_plugin(mei SOURCES src/plugin.cpp src/text_blocks.cpp src/text_input.cpp src/fonts.cpp)
target_link_libraries(mei PRIVATE port_search port_localisation port_text_drawing glyph_registry unicode_editor)

# The release package (`cmake --install build --component plugin --prefix dist/package`):
# mei/ as Anvil loads it, with the licences of the code it holds. Mei's fonts are built
# apart (tools/build-fonts.ps1) and go to the Workshop mod's gfx/fonts/mei/.
install(TARGETS mei RUNTIME DESTINATION mei COMPONENT plugin)
install(FILES plugin.toml symbols.toml settings.toml LICENSE DESTINATION mei COMPONENT plugin)
install(FILES ${UPSTREAM}/THIRD_PARTY_NOTICES.md DESTINATION mei/third-party COMPONENT plugin)
install(FILES ${UPSTREAM}/vendor/utfcpp/LICENSE DESTINATION mei/third-party RENAME utfcpp-LICENSE.txt COMPONENT plugin)

# The original's own unit tests of the parts this plugin uses: UTF-8 text, the
# EU4DLL escape conversion, search, pinyin and the glyph registry. They run on this build of them.
enable_testing()
foreach(test unicode_text unicode_services unicode_search native_search pinyin_fuzzy search_resources yaml_input
             legacy_text glyph_registry)
    add_executable(${test}_tests ${UPSTREAM}/tests/${test}_tests.cpp)
    set_target_properties(${test}_tests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
    target_compile_options(${test}_tests PRIVATE /W4 /utf-8 /EHsc /D_CRT_SECURE_NO_WARNINGS)
    add_test(NAME ${test} COMMAND ${test}_tests WORKING_DIRECTORY ${UPSTREAM})
endforeach()
target_link_libraries(unicode_text_tests PRIVATE unicode_text)
target_link_libraries(unicode_services_tests PRIVATE unicode_services)
target_link_libraries(yaml_input_tests PRIVATE unicode_text)
target_link_libraries(legacy_text_tests PRIVATE legacy_text)
target_link_libraries(glyph_registry_tests PRIVATE glyph_registry)
foreach(test unicode_search native_search pinyin_fuzzy search_resources)
    target_link_libraries(${test}_tests PRIVATE unicode_search)
endforeach()

# This plugin's own glue: the detours pick the same callers as the original, the
# localisation import keeps the game's text in UTF-8, and the text blocks decode it.
add_executable(port_tests tests/port_tests.cpp tests/localisation_tests.cpp tests/text_drawing_tests.cpp)
target_compile_features(port_tests PRIVATE cxx_std_20)
target_link_libraries(port_tests PRIVATE port_search port_localisation port_text_drawing)
target_compile_options(port_tests PRIVATE /W4 /utf-8 /EHsc)
add_test(NAME port COMMAND port_tests)
