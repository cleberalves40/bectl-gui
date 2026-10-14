#!/bin/sh
# run-valgrind.sh – debug de memória do bectl_gui com Valgrind
#
# FreeBSD 15.1:
#   pkg install valgrind
#
# Uso:
#   sh run-valgrind.sh              # memcheck completo
#   sh run-valgrind.sh --quick      # menos detalhe, mais rápido
#   sh run-valgrind.sh --gen-suppressions   # gera suppressions interativas

set -e
cd "$(dirname "$0")"

BIN="./build/bectl_gui"
if [ ! -x "$BIN" ]; then
    echo "Binário não encontrado: $BIN"
    echo "Compile antes:"
    echo "  sh fix-timestamps.sh"
    echo "  cmake -B build -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_COMPILER=clang++"
    echo "  cmake --build build -j\$(sysctl -n hw.ncpu)"
    exit 1
fi

if ! command -v valgrind >/dev/null 2>&1; then
    echo "valgrind não encontrado. Instale: pkg install valgrind"
    exit 1
fi

SUPP=""
if [ -f qt6.supp ]; then
    SUPP="--suppressions=qt6.supp"
fi

LOG="valgrind-bectl_gui.log"
MODE="${1:---full}"

case "$MODE" in
    --quick)
        echo "==> Valgrind (rápido) – log: $LOG"
        valgrind \
            --tool=memcheck \
            --leak-check=yes \
            --show-leak-kinds=definite,indirect \
            --track-origins=yes \
            --num-callers=20 \
            $SUPP \
            --log-file="$LOG" \
            "$BIN"
        ;;
    --gen-suppressions)
        echo "==> Valgrind com geração de suppressions"
        valgrind \
            --tool=memcheck \
            --leak-check=full \
            --show-leak-kinds=all \
            --gen-suppressions=all \
            --log-file="$LOG" \
            "$BIN"
        ;;
    *)
        echo "==> Valgrind (completo) – log: $LOG"
        valgrind \
            --tool=memcheck \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --num-callers=30 \
            --error-limit=no \
            $SUPP \
            --log-file="$LOG" \
            "$BIN"
        ;;
esac

echo ""
echo "Concluído. Resumo (últimas linhas do log):"
echo "----------------------------------------"
tail -n 40 "$LOG" 2>/dev/null || true
echo "----------------------------------------"
echo "Log completo: $LOG"
