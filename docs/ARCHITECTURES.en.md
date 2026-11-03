# Supported architectures

bectl-gui is validated on FreeBSD **Tier 1** architectures, according to the project's [platform page](https://www.freebsd.org/platforms/): **amd64** and **aarch64**, in the 14.x and 15.x series. i386 is no longer supported in FreeBSD 15; armv7, powerpc64, and riscv64 are Tier 2.

Each combination has its own job in the [Build & Test](../.github/workflows/build.yml) workflow. Jobs run in a [vmactions/freebsd-vm](https://github.com/vmactions/freebsd-vm) VM and execute [`ci/freebsd-ci.sh`](../ci/freebsd-ci.sh). The script builds the GUI and tests with the system Clang and Qt6 packages, then runs [`tests/freebsd-e2e.sh`](../tests/freebsd-e2e.sh).

## CI coverage

For each combination, in order:

1. **Build** with system Clang and Qt6 packages, without warnings.
2. **`tst_validation`**: name and path validation, arguments passed to `bectl`, exit status, message localization, parsing `bectl list -s`, and converting space values (`24K`, `1.69M`, `4.19G`) to bytes, using a fake `bectl`.
3. **`tst_mainwindow`**: the window with a fake `bectl` (offscreen platform). Checks responsiveness during execution; cached list use for filtering and selection; selection-aware actions and Snapshots tab; selection preservation on refresh; blocking other actions and closing during an operation; Preferences (system theme by default, save and restore); duplicate actions, shortcuts, and icons; both activation modes; and column sorting (space in bytes, date, state, saved order, restoring `bectl` order, and preserving selection). The Ubuntu job also uses `QT_STYLE_OVERRIDE=windows` to verify that switching back to **System** restores a style other than Fusion.
4. **`tst_bectlmanager`**: integration against real `bectl(8)`. [`tests/freebsd-e2e.sh`](../tests/freebsd-e2e.sh) creates a scratch ZFS pool on an `md(4)` device, configured as a Boot Environment root (`<pool>/ROOT/default`, with `bootfs`), and runs the test with `bectl -r <pool>/ROOT`, without touching system BEs. It covers listing BEs and snapshots; creation from a BE and snapshot; duplication; renaming; permanent and temporary activation; mounting; snapshot destruction; and bulk destruction.
5. **GUI startup** (offscreen).
6. **Port** ([`ci/freebsd-port.sh`](../ci/freebsd-port.sh)), using the ports framework in a sparse tree (`Mk/`, `Templates/`, `Keywords/`, and direct dependencies only): `portlint`, build, `stage-qa` with `DEVELOPER=yes`, `check-plist`, package creation, installation with `pkg add`, and launching the installed binary. The distfile is generated from the commit under test with the name and prefix expected by `USE_GITHUB`, because the release tag only exists after merge.

The port is not built with `poudriere`: that would require a jail and compiling dependencies, which is too expensive per job, especially on emulated aarch64. The validation above uses the same framework with dependencies installed by `pkg`.

## Results (1.2.0)

| Release | Architecture | Compiler | Qt6 | Warnings | Tests (validation + GUI + integration) | Port |
|:---|:---|:---|:---|:---:|:---:|:---|
| 14.5 | amd64 | Clang 21.1.8 | 6.11.2 | 0 | 32 + 23 + 14 ✅ | ✅ `FreeBSD:14:amd64` |
| 14.5 | aarch64 | Clang 21.1.8 | 6.11.2 | 0 | 32 + 23 + 14 ✅ | ✅ `FreeBSD:14:aarch64` |
| 15.1 | amd64 | Clang 19.1.7 | 6.11.2 | 0 | 32 + 23 + 14 ✅ | ✅ `FreeBSD:15:amd64` |
| 15.1 | aarch64 | Clang 19.1.7 | 6.11.2 | 0 | 32 + 23 + 14 ✅ | ✅ `FreeBSD:15:aarch64` |

No combination required code changes or platform-specific flags. The code is pure Qt and does not depend on type sizes, byte order, or `char` signedness; its only system integration is running `bectl(8)` through `QProcess`.

On Ubuntu, the same tests (except integration, which requires `bectl`) also run with AddressSanitizer and UBSan.

Port validation found two issues, both fixed in 0.2.1:

- The binary links to `libGLX` and `libOpenGL` (through Qt6 Gui), which were undeclared in the port (`stage-qa` error). Fixed with `USE_GL=gl opengl`.
- `USE_QT=tools:build` declared unused `qt6-tools`.

`portlint` finishes without fatal errors and reports two inapplicable warnings: `MimeType` (the `.desktop` file does not declare MIME types) and the `DEVELOPER=yes` suggestion, which CI already uses.

Notes:

- FreeBSD 14.5 ships a newer Clang (21.1.8) than 15.1 (19.1.7). Both series remain in the matrix despite having no code differences.
- aarch64 runs emulated (TCG) on an x86_64 runner, as recommended by vmactions. Jobs take 25-45 minutes, compared with about 3 minutes on amd64.
- Releases without security support leave the matrix. For example, 14.4 reaches end of support on 2026-12-31 and is replaced by 14.5.

## Running locally

On FreeBSD amd64 or aarch64, as root and from the source root:

```sh
sh ci/freebsd-ci.sh                # installs dependencies, builds, and tests
sh tests/freebsd-e2e.sh build       # tests only, using a build with -DBECTL_GUI_BUILD_TESTS=ON
sh ci/freebsd-port.sh               # validates the port (after ci/freebsd-ci.sh)
```

The test loads the `zfs` module if needed and uses a 1 GB file under `/var/tmp`, removed at the end.
