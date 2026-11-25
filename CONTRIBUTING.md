# Contributing

Every change goes to `main` through a pull request, which needs the `windows` (build)
and `conventional-title` checks to pass and is **squash-merged**: its title becomes the
commit on `main` and the changelog entry.

Titles follow [Conventional Commits](https://www.conventionalcommits.org/):
`<type>(<optional scope>): <summary in the imperative, lowercase, no period>`.

| Type | Use for | Release |
| --- | --- | --- |
| `feat` | something new for players | minor |
| `fix` | a bug fix | patch |
| `perf` | faster or lighter, same behaviour | patch |
| `revert` | reverting an earlier change | patch |
| `build` | CMake, the SDK version, packaging | patch |
| `docs`, `refactor`, `test`, `ci`, `chore` | everything else | none |

Before 1.0.0 a breaking change (`feat!:` with a `BREAKING CHANGE:` paragraph) bumps the
minor version.

When you change `symbols.toml`, run the `-check` target with `EU4_EXE` set and commit
the new `symbols.lock` in the same pull request; CI fails on a stale lock.

**Game code and names.** This repository holds no code of eu4.exe and no names from its debug symbols.
`symbols.toml` verifies each address with `sha256` and `size`, not with the game's bytes; write `bytes` while you
work and run Anvil's `tools/symbols-to-digest.py` before you commit. Name the game's functions, classes and
fields by their address and what they do, unless eu4.exe itself shows the name (its RTTI or its own log and
assert texts). Our own symbol keys, addresses, offsets and signatures are fine. In Mei, `tools/gen-symbols.py`
writes the digests itself: change the generator, not `symbols.toml`.
