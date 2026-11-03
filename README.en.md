# bectl GUI (C++/Qt6) - Boot Environment Manager for FreeBSD

[Português](README.md) | **English**

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Framework](https://img.shields.io/badge/Framework-Qt6-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/Platform-FreeBSD-red.svg)](https://www.freebsd.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build & Test](https://github.com/cleberalves40/bectl-gui/actions/workflows/build.yml/badge.svg)](https://github.com/cleberalves40/bectl-gui/actions/workflows/build.yml)

A native **C++ / Qt6** graphical interface for FreeBSD's [`bectl(8)`](https://man.freebsd.org/cgi/man.cgi?bectl(8)) utility, for managing ZFS Boot Environments quickly, safely, and intuitively.

Developed and optimized for **FreeBSD 14/15** with the native Clang/LLVM compiler.

## Architecture

```text
bectl-gui/
├── CMakeLists.txt           # Main build system (CMake)
├── bectl_gui.pro            # Alternative build system (QMake)
├── bectl_gui.desktop        # Desktop application menu entry
├── CHANGELOG.md             # Release history
├── freebsd/                 # FreeBSD Ports port (sysutils/bectl-gui)
│   ├── Makefile
│   └── pkg-descr
├── ci/                      # Scripts run in FreeBSD CI VMs
│   ├── freebsd-ci.sh        # Build and tests
│   └── freebsd-port.sh      # Port validation (portlint, stage-qa, package)
├── docs/
│   └── ARCHITECTURES.md     # Tier 1 architecture validation
├── tests/
│   ├── tst_validation.cpp   # Validation and messages (fake bectl)
│   ├── tst_mainwindow.cpp   # Interface (fake bectl, offscreen)
│   ├── tst_bectlmanager.cpp # Integration with real bectl(8)
│   └── freebsd-e2e.sh       # Creates a scratch ZFS pool and runs tests
├── .github/workflows/
│   └── build.yml            # Ubuntu + FreeBSD 14.5/15.1 x amd64/aarch64
└── src/
    ├── main.cpp             # Entry point (QApplication and i18n)
    ├── logic/               # Logic and system integration layer
    │   ├── bectlmanager.h   # bectl and qt-sudo command wrapper
    │   └── bectlmanager.cpp
    ├── gui/                 # Qt6 interface (OctoPKG style)
    │   ├── mainwindow.h
    │   ├── mainwindow.cpp
    │   ├── themes.h         # Palettes (Catppuccin, Nord, Dracula, etc.)
    │   └── themes.cpp
    └── i18n/                # Localization (en_US, pt_BR, es_ES)
        ├── i18n.h
        └── i18n.cpp
```

| Component | Location | Responsibility |
|:---|:---|:---|
| **Logic** | `src/logic/bectlmanager.*` | Run `bectl` via `QProcess`, validate names, and parse output (BEs and snapshots) |
| **GUI** | `src/gui/mainwindow.*` | Qt6 window, background operations, Info / Output / Help / Snapshots tabs |
| **Themes** | `src/gui/themes.*` | QSS stylesheets for more than 10 themes |
| **i18n** | `src/i18n/i18n.*` | Dynamic localization (en_US, pt_BR, es_ES), including error messages |
| **Entry point** | `src/main.cpp` | Initialize `QApplication` and locale |

The code builds two static libraries, `bectl_logic` and `bectl_ui`, shared by the executable and tests.

### Privilege elevation with qt-sudo

All write operations requiring superuser privileges are run transparently through `qt-sudo`:

```text
qt-sudo bectl create be-name
qt-sudo bectl activate be-name
qt-sudo bectl destroy -F be-name
```

## Features

- **Complete listing**: lists all Boot Environments (`bectl list -H`) with space usage and mount point.
- **Column sorting**: click the Boot Environment, Active, Mount, Space, or Created header to sort; click again to reverse, and a third time to restore `bectl` order. Space sorts by actual size (5M < 10M < 1.2G), and Created chronologically. The chosen order is remembered. The Snapshots tab can also be sorted.
- **Visual status**:
  - Green: active now (`N` - Now).
  - Yellow: active on next boot (`R` - Reboot).
- **Flexible creation**: create BEs from a custom origin, with recursive creation (`-r`).
- **Activation**: permanently or temporarily activate for the next boot (`-t`).
- **Renaming**: rename existing BEs after validation.
- **Snapshots**: view snapshots for the selected BE (or all BEs when there is no single selection), including size and date; create, clone into a new BE, and destroy snapshots.
- **Mounting**: mount a BE to a directory (or a temporary directory created by `bectl`) to inspect its files, then unmount it.
- **Responsive interface**: operations run in the background; the window remains responsive while `bectl` runs and `qt-sudo` prompts for a password.
- **Bulk destruction**: select multiple BEs with `Ctrl+Click` or `Shift+Click`.
- **Active BE protection**: prevents accidental deletion of the currently active Boot Environment.
- **Multilingual**: English, Brazilian Portuguese, and Spanish; follows the system language or the choice in **Edit > Preferences**.
- **Themes**: follows the Qt6 desktop theme by default (for example, one configured in qt6ct). Built-in themes are also available under **Edit > Preferences**: OctoPKG, Catppuccin (Frappé, Latte, Mocha, Macchiato), Solarized, Selenized, Dracula, Tokyo Night, Nord, Gruvbox, and One Dark. Theme and language choices are remembered.

## Requirements

On **FreeBSD**:

```sh
pkg install cmake ninja
pkg install qt6-base
pkg install qt-sudo
```

## Build and install

### CMake + Ninja (recommended)

```sh
# 1. Align timestamps (avoids 'Clock skew' warnings on ZFS/NFS mounts)
sh fix-timestamps.sh

# 2. Configure
cmake -B build -S . -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=clang++

# 3. Build
cmake --build build -j$(sysctl -n hw.ncpu)

# 4. Run
./build/bectl_gui

# 5. Optional: install to /usr/local/bin and share/applications
cmake --install build --prefix /usr/local
```

### Traditional Make workflow

```sh
sh fix-timestamps.sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
cmake --build build -j$(sysctl -n hw.ncpu)
./build/bectl_gui
```

### QMake

```sh
mkdir -p build && cd build
qmake6 ../bectl_gui.pro
make -j$(sysctl -n hw.ncpu)
./bectl_gui
```

### Tests

On FreeBSD, as root, the integration test runs against real `bectl(8)` in a scratch ZFS pool without touching the system's Boot Environments:

```sh
cmake -B build -S . -G Ninja -DBECTL_GUI_BUILD_TESTS=ON
cmake --build build
sh tests/freebsd-e2e.sh build
```

## Supported platforms

Validated on FreeBSD Tier 1 **amd64** and **aarch64**, releases **14.5** and **15.1**. Each combination has a CI job that builds and runs the integration test. Results and details are in [docs/ARCHITECTURES.en.md](docs/ARCHITECTURES.en.md), release history in [CHANGELOG.en.md](CHANGELOG.en.md), and versioning and port-submission instructions in [docs/RELEASING.en.md](docs/RELEASING.en.md).

## Keyboard shortcuts

| Shortcut | Action |
|:---|:---|
| <kbd>F5</kbd> or <kbd>Ctrl+R</kbd> | Refresh the BE list |
| <kbd>Ctrl+N</kbd> | Create a Boot Environment |
| <kbd>Ctrl+Enter</kbd> | Activate the selected BE |
| <kbd>Ctrl+T</kbd> | Activate temporarily (next boot) |
| <kbd>F2</kbd> | Rename a BE |
| <kbd>Ctrl+Shift+S</kbd> | Create a snapshot |
| <kbd>Ctrl+M</kbd> | Mount the selected BE |
| <kbd>Ctrl+U</kbd> | Unmount the selected BE |
| <kbd>Delete</kbd> | Destroy selected BE(s) |
| <kbd>Ctrl+F</kbd> | Focus the search/filter field |
| <kbd>Ctrl+,</kbd> | Preferences (theme and language) |
| <kbd>Ctrl+Shift+T</kbd> | Switch to the next theme |
| <kbd>Ctrl+1 / 2 / 3</kbd> | Switch tabs (Info / Output / Help) |
| <kbd>Ctrl+4</kbd> | Snapshots tab |
| <kbd>Ctrl+Q</kbd> | Quit |

## Security

- The application **never runs as root**. The GUI runs with the logged-in user's credentials.
- Only `bectl` write operations invoke `qt-sudo`, when needed.
- BE deletion requires explicit confirmation in the GUI and is blocked for the active BE.

## License

Distributed under the [MIT](LICENSE) license.
