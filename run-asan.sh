#!/bin/sh
# run-asan.sh – alternativa ao Valgrind usando AddressSanitizer (Clang)
# Frequentemente mais estável no FreeBSD com Qt do que o Valgrind.
#
# Uso: sh run-asan.sh

set -e
cd "$(dirname "$0")"

echo "==> Recompilando com AddressSanitizer (Debug)..."
rm -rf build-asan
cmake -B build-asan -S . \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer -g" \
    -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"

cmake --build build-asan -j"$(sysctl -n hw.ncpu 2>/dev/null || echo 2)"

echo ""
echo "==> Executando com ASan..."
echo "    (defina ASAN_OPTIONS se quiser mais detalhe)"
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=1:halt_on_error=0:abort_on_error=0}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1}"

./build-asan/bectl_gui
