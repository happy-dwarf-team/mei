# Changelog

## [0.5.0](https://github.com/happy-dwarf-team/mei/compare/v0.4.0...v0.5.0) (2026-11-09)

### Features

* Save names keep Chinese, Japanese and Korean characters. Before, the game turned each such character into one Latin letter, so the Ottomans' (奥斯曼) default save with a Chinese translation mod was `aaa1444_11_11.eu4`. Now the name stays as the game shows it, for example `俄罗斯1719_03_17.eu4` for Russia, and the save and load menus show and load such names. The new setting `utf8_save_names` (on) turns it off. Saves named this way show garbled names in a game that uses EU4DLL, and a multiplayer group should switch together. Tested in the lab on Steam with Chinese translation mods: the load menu lists and loads a Chinese-named save, and saving over it asks first and writes the UTF-8 name. Not tested: ironman, and GOG in game. ([#51](https://github.com/happy-dwarf-team/mei/issues/51))

## [0.4.0](https://github.com/happy-dwarf-team/mei/compare/v0.3.2...v0.4.0) (2026-11-09)

### Features

* Mei runs on the GOG build 1.37.5.0, the version GOG Galaxy installs when you roll EU4 back. On that build it loads with Anvil 1.15.0 or later, which knows the build. Tested in the lab on a copy of a GOG 1.37.5.0 install together with Bellows, Tongs and Hamweard. ([#46](https://github.com/happy-dwarf-team/mei/issues/46))

### Bug Fixes

* Steam friends see your country's name in your Steam status as you see it in the game. Before, a Chinese, Japanese or Korean name from a translation mod showed up as Latin letters such as `å¤§æ˜Ž`. Steam only; GOG has no Steam status. Checked in the lab that the game's call goes through Mei; the status itself was not looked at from a friend's account. ([#49](https://github.com/happy-dwarf-team/mei/issues/49))
* Text the game strips of colour, such as the name of a hired mercenary company and some tooltips, no longer loses half of a Chinese, Japanese or Korean character or keeps a stray byte after an English coloured name. This follows the fix in EU4 Unicode Patch 0.1.16, which Mei now builds on. ([#50](https://github.com/happy-dwarf-team/mei/issues/50))

## [0.3.2](https://github.com/happy-dwarf-team/mei/compare/v0.3.1...v0.3.2) (2026-11-09)


### Bug Fixes

* end a line after the glyph where the rules allow a break ([#47](https://github.com/happy-dwarf-team/mei/issues/47)) ([eed4052](https://github.com/happy-dwarf-team/mei/commit/eed40522ad4d225a6e275d18de274f6b0b288fc8))

## [0.3.1](https://github.com/happy-dwarf-team/mei/compare/v0.3.0...v0.3.1) (2026-11-08)


### Bug Fixes

* centre and wrap text with characters ending in a format byte ([#40](https://github.com/happy-dwarf-team/mei/issues/40)) ([8982797](https://github.com/happy-dwarf-team/mei/commit/8982797b1e58cf277befb836a2d0d8fe792f5d73)), closes [#38](https://github.com/happy-dwarf-team/mei/issues/38)

## [0.3.0](https://github.com/happy-dwarf-team/mei/compare/v0.2.0...v0.3.0) (2026-11-04)


### Features

* port the rest of the original's text editor ([#22](https://github.com/happy-dwarf-team/mei/issues/22)) ([59e4c79](https://github.com/happy-dwarf-team/mei/commit/59e4c792ca0ab2e54659d193c83bfacca2c6376c))
* read UTF-8 and EU4DLL names in scripts and saves ([#19](https://github.com/happy-dwarf-team/mei/issues/19)) ([cb4fbee](https://github.com/happy-dwarf-team/mei/commit/cb4fbee2f65e21620963353b446f7c6e8a09d865))


### Bug Fixes

* keep the text state per thread, as the original does ([#20](https://github.com/happy-dwarf-team/mei/issues/20)) ([48de8f7](https://github.com/happy-dwarf-team/mei/commit/48de8f751752205c919acc31c49ab93115795cdc))

## [0.2.0](https://github.com/happy-dwarf-team/mei/compare/v0.1.0...v0.2.0) (2026-11-03)


### Features

* run on the GOG build 1.37.5.2 ([#15](https://github.com/happy-dwarf-team/mei/issues/15)) ([a3dc709](https://github.com/happy-dwarf-team/mei/commit/a3dc709f7b6dd0513f0398fd6116200a01eac281))

## 0.1.0 (2026-11-02)


### Features

* decode UTF-8 in the map label blocks ([#4](https://github.com/happy-dwarf-team/mei/issues/4)) ([2ccc09f](https://github.com/happy-dwarf-team/mei/commit/2ccc09f63da68c681981991ee09d5fbf6e37e335))
* draw UTF-8 text with the game's fonts ([#2](https://github.com/happy-dwarf-team/mei/issues/2)) ([581a890](https://github.com/happy-dwarf-team/mei/commit/581a890d9a2be9cde7814f64b2ef0b4ba53ec33c))
* keep fonts' glyphs above U+00FF ([#3](https://github.com/happy-dwarf-team/mei/issues/3)) ([c03c822](https://github.com/happy-dwarf-team/mei/commit/c03c822876921dae87783cd485738e63895e56e9))
* port the original's localisation import, off by default ([#1](https://github.com/happy-dwarf-team/mei/issues/1)) ([54a9d3f](https://github.com/happy-dwarf-team/mei/commit/54a9d3f12067f16a4c2ce1bbc3c0cc6d88ae8e80))
* release as Mei, with a Steam Workshop mod ([#7](https://github.com/happy-dwarf-team/mei/issues/7)) ([0d3574c](https://github.com/happy-dwarf-team/mei/commit/0d3574ce7c0fc5351a25fd5a86f44066f18bf6d1))
* start the Anvil port of EU4 Unicode Patch with its search ([565ba4a](https://github.com/happy-dwarf-team/mei/commit/565ba4ae9e9c61495cf9fa0c0d6fc07f4faa7969))
* type UTF-8 text into text fields ([#5](https://github.com/happy-dwarf-team/mei/issues/5)) ([d45f7cc](https://github.com/happy-dwarf-team/mei/commit/d45f7ccbc597bf654cf84f749d77825e41799133))
* work with translation mods without configuration ([#6](https://github.com/happy-dwarf-team/mei/issues/6)) ([f883c3b](https://github.com/happy-dwarf-team/mei/commit/f883c3b58d411ce1617a9681545839845fed03b4))


### Bug Fixes

* convert the game's windows-1252 text before the original's search ([587ab6e](https://github.com/happy-dwarf-team/mei/commit/587ab6e88a58f02c9b2c44a51e7cccb2fcb7e1a6))


### Build

* compile the original's code as C++17 and fix the plugin's log call ([633ff58](https://github.com/happy-dwarf-team/mei/commit/633ff585d4e419912d317358a88b92c3265444c4))
