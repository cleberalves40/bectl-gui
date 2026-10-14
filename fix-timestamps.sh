#!/bin/sh
# Corrige timestamps para a hora atual do sistema.
# Execute após extrair o zip se aparecer "Clock skew detected".
#
# Uso: sh fix-timestamps.sh

cd "$(dirname "$0")" || exit 1
find . -type f -exec touch {} +
find . -type d -exec touch {} +
echo "Timestamps atualizados para: $(date)"
