# Changelog

All notable changes to bectl-gui are recorded here. This changelog follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and the project follows [Semantic Versioning](https://semver.org/) with `vX.Y.Z` tags.

[Português](CHANGELOG.md) | **English**

## [1.2.0] - 2026-10-18

### Added
- **Column sorting**: click the Boot Environment, Active, Mount, Space, or Created header to sort; a second click reverses the order and a third restores `bectl` order. The icon column sorts by state (in use, next boot, inactive). Space sorts by actual size (`5M` < `10M` < `1.2G`), not as text; Created sorts chronologically. The chosen order is saved with the other preferences and applies on subsequent runs; selection is preserved while sorting and refreshing. The Snapshots table is also sortable.
- `bectlSizeToBytes()`, which converts sizes printed by `bectl` (`humanize_number`: `24K`, `1.69M`, `4.19G`) to bytes.
- Tests: 10 additional `tst_validation` cases (size conversion; 32 total) and 5 additional `tst_mainwindow` cases (sorting; 23 total).

### Changed
- Help now describes sorting in all three languages.

## [1.1.2] - 2026-10-18

### Fixed
- BE selection was lost on every list refresh, including after each operation, and returned to the first BE. A subsequent action such as activating temporarily after activation could affect a different BE. Refresh and filtering now preserve selected BEs, selecting the first only if none remain visible.
- **Clone snapshot** used the same icon as **Create BE**, and **Destroy snapshot** used the same icon as **Destroy BE**; each action now has its own icon.

### Changed
- Removed duplicated GUI code: selection checks ("select exactly one BE", repeated in five actions, and "select a snapshot", in two) and the nearly identical Activate and Activate Temporarily actions now each have one implementation. Text and behavior are unchanged.
- Updated `docs/ARCHITECTURES.md` with current test coverage.

### Added
- Three `tst_mainwindow` cases (18 total): selection preserved after refresh and filtering, distinct icons, and both activation modes invoking `bectl` with the correct arguments.

## [1.1.1] - 2026-10-18

### Fixed
- **Create snapshot** appeared in three places: Actions menu, main toolbar, and Snapshots tab toolbar. It was removed from the main toolbar and remains in the menu and Snapshots tab, alongside Clone and Destroy snapshot.
- In the **Actions** menu, Create snapshot was separated from Clone and Destroy snapshot; all three now form a group.
- The **Refresh** shortcut was defined twice in succession (`F5`, then `QKeySequence::Refresh` + `Ctrl+R`, which varied by desktop environment). It is now defined once as `F5` and `Ctrl+R`.

### Added
- `tst_mainwindow` checks main toolbar composition, the snapshot group in the Actions menu, and that no keyboard shortcut triggers two actions.

## [1.1.0] - 2026-10-18

### Added
- **Edit > Preferences...** (`Ctrl+,`): a window with **Theme** and **Language** dropdowns. Choices are saved with `QSettings` and restored on subsequent runs; previously theme and language reset on every launch.
- **System** theme, now the default: with no custom stylesheet, the interface follows the Qt6 style and palette set by the desktop environment (for example, Catppuccin Frappé configured in qt6ct). Built-in themes remain available in Preferences; selecting **System** restores the desktop style.
- **System default** language, which follows the desktop environment as before.

### Changed
- **Theme** and **Language** menus moved from the menu bar to **Edit > Preferences**. `Ctrl+Shift+T` still cycles themes and now also saves the choice.
- Without a saved preference, the initial theme is **System** instead of OctoPKG.

### Removed
- `image.png`, an outdated v0.1.0 screenshot, to be replaced with one from the current version.

### Fixed
- The `tst_mainwindow` non-blocking test measured constructor time (<800 ms) and failed on emulated aarch64 CI (862 ms). It now checks that the table is empty immediately after construction, which does not depend on machine speed.

## [1.0.0] - 2026-10-18

First stable release. Since 0.1.0: validation on FreeBSD Tier 1 architectures (amd64 and aarch64, releases 14.5 and 15.1), name validation before calling `bectl`, a responsive interface during operations, mounting, snapshots, and a validated port. Details are in the entries below.

### Added
- CI: tests also run under AddressSanitizer and UBSan in the Ubuntu job; memory errors, leaks, and undefined behavior fail the build.
- `docs/RELEASING.md`: versioning process and steps for submitting the port to the official FreeBSD tree.

### Changed
- Port `MAINTAINER` is now `cleber@bsd.com.br`, a valid address for the Ports tree.
- `pkg-descr` now describes current features (snapshots, mounting, validation, background operations).
- The **Keyboard shortcuts** dialog is localized (en_US, pt_BR, es_ES) and lists all shortcuts; it was previously English-only.
- Updated the README's repository structure and component descriptions.

## [0.3.0] - 2026-10-18

### Added
- **Snapshots** tab (`Ctrl+4`): lists snapshots for the selected BE (or all snapshots when no single BE is selected), with size and creation date; allows creating, cloning into a new BE, and destroying snapshots. Previously, a snapshot could be created but not viewed in the interface.
- `BectlManager::listSnapshots()` parses `bectl list -H -s` according to `sbin/bectl/bectl_list.c`, ignoring clone origins that also contain `@`. Covered by a unit test with `bectl`-formatted output and by integration tests against real `bectl`.

### Changed
- Creating a snapshot refreshes the list so it appears in the new tab.

## [0.2.1] - 2026-10-17

### Fixed
- Port: the binary links to `libGLX` and `libOpenGL` (from `graphics/libglvnd` through Qt6 Gui), but the port did not declare them; `stage-qa` failed. Added `USES=gl` and `USE_GL=gl opengl`.
- Port: removed the unused `qt6-tools` build dependency (`USE_QT=tools:build`), which also pulled in `qt6-declarative` and other unnecessary packages. The README and `CMakeLists.txt` no longer request it.

### Added
- `ci/freebsd-port.sh`: port-framework validation in all four FreeBSD Tier 1 jobs: `portlint`, build, `stage-qa` (with `DEVELOPER=yes`), `check-plist`, package creation and installation, and launching the installed binary. The distfile is generated from the commit under test.

## [0.2.0] - 2026-10-17

### Added
- **Create snapshot** (`Ctrl+Shift+S`): creates `be@snapshot`, optionally recursively. To clone a snapshot, the **Create** origin field now accepts a typed `be@snapshot`.
- **Mount** (`Ctrl+M`) and **Unmount** (`Ctrl+U`): mount a BE to a chosen directory or a temporary directory created by `bectl`.
- Actions follow the selection: activate, rename, and mount require one BE; unmount applies only to a mounted BE that is not currently in use.
- `tests/tst_mainwindow.cpp`: GUI test with a fake `bectl`, runnable on any system.

### Changed
- Operations run on a separate thread. The window stays responsive while `bectl` runs and `qt-sudo` waits for a password. While an operation runs, `bectl` actions (including shortcuts) are disabled, and closing the window asks the user to wait.
- Filtering, selection, and theme/language changes use the most recent listing. Previously, every filter keystroke and table click ran `bectl list`.

### Fixed
- In English, the **Help** tab showed "Help body" instead of its contents. Help now documents the new operations in all three languages.

## [0.1.2] - 2026-10-17

### Fixed
- Privileged commands no longer have a 60-second timeout. The timeout included the time taken to type the password in `qt-sudo`, canceling slower operations; long operations such as `destroy` could also be interrupted. The timeout still applies to unprivileged reads.
- Boot Environment and snapshot names are validated before reaching `bectl(8)`, using the same rules as `libbe` (letters, digits, and `_-.:`). A name starting with `-` could be passed to root-run `bectl` and interpreted as an option. Mount points must be absolute.
- `BectlManager` error messages were hard-coded in Portuguese; they now follow the interface language (en_US, pt_BR, es_ES).

### Added
- `tests/tst_validation.cpp`: unit tests for validation and messages, runnable on any system, including the Ubuntu CI job.

## [0.1.1] - 2026-10-17

### Added
- Validation on FreeBSD Tier 1 architectures (amd64 and aarch64), releases 14.5 and 15.1, with a CI job for each combination. All four build without warnings and pass integration tests with no code or flag changes.
- `tests/tst_bectlmanager.cpp` and `tests/freebsd-e2e.sh`: integration test of `BectlManager` against real `bectl(8)` in a scratch ZFS pool, plus a GUI startup test.
- `docs/ARCHITECTURES.md`: results by architecture and release.

### Fixed
- The project version now has one source (`project(... VERSION ...)` in `CMakeLists.txt`), shown in About and `QApplication::applicationVersion()`. Previously, CMake and the port declared 1.0.0 while About mentioned only "FreeBSD 15.1".
- `freebsd/Makefile`: set `DISTVERSIONPREFIX=v` so `USE_GITHUB` downloads `vX.Y.Z` tags.

## [0.1.0] - 2026-10-17

### Added
- First tagged release: list, create (with origin and `-r`), activate (permanently or temporarily), rename, and bulk-destroy Boot Environments through `bectl(8)`, with privilege elevation through `qt-sudo`.
- Themes, localization (en_US, pt_BR, es_ES), CMake and QMake builds, FreeBSD port template, and GitHub Actions CI (Ubuntu and FreeBSD amd64).

[1.2.0]: https://github.com/cleberalves40/bectl-gui/compare/v1.1.2...v1.2.0
[1.1.2]: https://github.com/cleberalves40/bectl-gui/compare/v1.1.1...v1.1.2
[1.1.1]: https://github.com/cleberalves40/bectl-gui/compare/v1.1.0...v1.1.1
[1.1.0]: https://github.com/cleberalves40/bectl-gui/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/cleberalves40/bectl-gui/compare/v0.3.0...v1.0.0
[0.3.0]: https://github.com/cleberalves40/bectl-gui/compare/v0.2.1...v0.3.0
[0.2.1]: https://github.com/cleberalves40/bectl-gui/compare/v0.2.0...v0.2.1
[0.2.0]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.2...v0.2.0
[0.1.2]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.1...v0.1.2
[0.1.1]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/cleberalves40/bectl-gui/releases/tag/v0.1.0
