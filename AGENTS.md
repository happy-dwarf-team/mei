# Mei: repository rules

- Anvil plugin that lets EU4 show, wrap, search and type Chinese, Japanese and Korean text. It
  translates nothing. It is a port of EU4 Unicode Patch by VulonLok (MIT): keep the notice.
- [CONTRIBUTING.md](CONTRIBUTING.md) lists the title types, the required checks and
  `symbols.toml` with its lock. Run the build and `ctest` before finishing a change; CI runs them.
- A change to the Workshop description changes all three languages, and the Chinese ones reach
  Steam only from client mode (README, "Steam Workshop").
- English in code and docs, apart from the Chinese descriptions. Do not imply affiliation with
  Paradox Interactive.
- The agent owns the pull request flow: it works on a branch with Conventional Commits,
  pushes it and opens the PR as a draft. Once ready (green CI, the change tested as above,
  no blocking review findings, not put on hold by the user) it marks the PR ready and
  squash-merges it; GitHub does not allow auto-merge on a draft. Pull requests from other
  contributors get a review and a recommendation; the maintainer decides on merging them.
  Do not bypass CI or branch protection, and do not change repository settings beyond what
  auto-merge needs.
