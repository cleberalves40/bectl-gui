#!/bin/sh
#
# Teste ponta a ponta no FreeBSD: cria um pool ZFS de rascunho num md(4)
# organizado como raiz de boot environments, executa tst_bectlmanager contra
# ele por meio de "bectl -r" e verifica que a GUI inicia. Precisa de root.
# Os boot environments do sistema não são tocados.
#
# uso: tests/freebsd-e2e.sh [diretório-de-build]

set -eu

build=$(cd "${1:-build}" && pwd)

step() { echo "== $*"; }
fail() { echo "FAIL: $*" >&2; exit 1; }

[ "$(id -u)" -eq 0 ] || fail "precisa ser executado como root"
[ -x "$build/tst_bectlmanager" ] || fail "falta $build/tst_bectlmanager"

pool=bectltest$$
work=$(mktemp -d /var/tmp/bectl-e2e.XXXXXX)
md=

cleanup() {
	set +e
	zpool destroy -f "$pool" 2>/dev/null
	[ -n "$md" ] && mdconfig -d -u "$md" 2>/dev/null
	rm -rf "$work"
}
trap cleanup EXIT

step "ZFS"
kldstat -q -m zfs || kldload zfs

step "pool de rascunho $pool"
truncate -s 1g "$work/pool.img"
md=$(mdconfig -a -t vnode -f "$work/pool.img")
# -R: altroot, para que nada do pool seja montado sobre o sistema.
zpool create -R "$work/altroot" -O mountpoint=none "$pool" "/dev/$md"
zfs create -o mountpoint=none "$pool/ROOT"
zfs create -o mountpoint=/ -o canmount=noauto "$pool/ROOT/default"
zpool set bootfs="$pool/ROOT/default" "$pool"

cat >"$work/bectl" <<EOF
#!/bin/sh
exec /sbin/bectl -r $pool/ROOT "\$@"
EOF
chmod +x "$work/bectl"
"$work/bectl" list

step "tst_bectlmanager"
BECTL_TEST_CMD=$work/bectl "$build/tst_bectlmanager"

if [ -x "$build/bectl_gui" ]; then
	# A GUI não tem modo batch: precisa continuar de pé até o timeout(1)
	# encerrá-la (status 124). Qualquer outro status indica falha ou crash.
	step "inicialização da GUI (offscreen)"
	set +e
	QT_QPA_PLATFORM=offscreen timeout 20 "$build/bectl_gui" >"$work/gui.log" 2>&1
	rc=$?
	set -e
	cat "$work/gui.log"
	[ "$rc" -eq 124 ] || fail "a GUI terminou com status $rc"
fi

step "todos os testes passaram"
