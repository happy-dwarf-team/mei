# Changelog

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
