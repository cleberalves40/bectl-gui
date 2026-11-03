# Versioning and port submission

[Português](RELEASING.md) | **English**

## Versioning

The project follows [Semantic Versioning](https://semver.org/) with `vX.Y.Z` tags:

- **patch** (`v1.0.1`): fixes, no new features;
- **minor** (`v1.1.0`): new backward-compatible features;
- **major** (`v2.0.0`): incompatible changes.

The version is recorded in three files. CI fails if they differ or if `CHANGELOG.md` is missing the corresponding entry:

| File | Field |
|:---|:---|
| `CMakeLists.txt` | `project(bectl_gui VERSION X.Y.Z ...)`: source for the version shown in the program |
| `freebsd/Makefile` | `DISTVERSION=` |
| `bectl_gui.pro` | `DEFINES += BECTL_GUI_VERSION=...` |

## Publishing a release

1. On a `release-X.Y.Z` branch, update all three files and record the changes in `CHANGELOG.md` using [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), with a comparison link at the end.
2. Open a pull request. CI must pass on Ubuntu (including ASan and UBSan tests) and on all four FreeBSD Tier 1 jobs, which include port validation.
3. Merge using a *merge commit*.
4. Create an annotated tag on the merge commit and publish the release with the CHANGELOG text:

   ```sh
   git fetch origin
   git tag -a vX.Y.Z -m "bectl-gui vX.Y.Z" origin/main
   git push origin vX.Y.Z
   ```

## Submitting the port to FreeBSD

The port is in `freebsd/` and is validated on every pull request (see [ARCHITECTURES.en.md](ARCHITECTURES.en.md)). To submit it to the official tree as `sysutils/bectl-gui`, after publishing the tag:

1. On a FreeBSD machine with an up-to-date ports tree, copy `freebsd/Makefile` and `freebsd/pkg-descr` to `/usr/ports/sysutils/bectl-gui/` and generate `distinfo` from the published tag tarball:

   ```sh
   cd /usr/ports/sysutils/bectl-gui
   make makesum
   ```

2. Test in a clean environment on as many architectures as possible (CI does not use poudriere):

   ```sh
   portlint -AC
   poudriere testport -j <jail> -o sysutils/bectl-gui
   ```

3. Add `bectl-gui` to `SUBDIR` in `sysutils/Makefile` and generate the patch with `git format-patch` from the git-based ports tree.
4. File a report at <https://bugs.freebsd.org/> (product *Ports & Packages*, component *Individual Port(s)*), titled `[NEW PORT] sysutils/bectl-gui: Qt6 graphical front-end for bectl(8)`, and attach the patch and `poudriere testport` log.

The port's `MAINTAINER` is `cleber@bsd.com.br`, who will receive report and update notifications.
