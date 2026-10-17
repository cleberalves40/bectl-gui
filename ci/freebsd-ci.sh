#!/bin/sh
#
# Compila o bectl-gui no FreeBSD e executa tests/freebsd-e2e.sh. Roda como
# root dentro da VM de CI, a partir da raiz do código-fonte.

set -eu

echo "== $(uname -srm), $(sysctl -n hw.ncpu) CPU(s), $(($(sysctl -n hw.physmem) / 1048576)) MB"

pkg install -y cmake ninja qt6-base qt6-tools >/dev/null

echo "== toolchain"
c++ --version | head -1
cmake --version | head -1
pkg query '%n-%v' qt6-base

echo "== build"
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release -DBECTL_GUI_BUILD_TESTS=ON
if ! cmake --build build >build.log 2>&1; then
	cat build.log
	exit 1
fi
cat build.log
echo "== warnings do compilador: $(grep -c 'warning:' build.log || true)"
file build/bectl_gui build/tst_bectlmanager

sh tests/freebsd-e2e.sh build
