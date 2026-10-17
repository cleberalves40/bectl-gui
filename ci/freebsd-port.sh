#!/bin/sh
#
# Valida o port (freebsd/) com o framework de ports do FreeBSD: portlint,
# build, stage-qa, check-plist, geração do pacote, instalação do pacote e
# inicialização do binário instalado. Roda como root na VM de CI, depois de
# ci/freebsd-ci.sh (que já instalou as dependências de build), a partir da
# raiz do código-fonte.
#
# O distfile é gerado do commit em teste, com o nome e o prefixo que o
# USE_GITHUB espera: a tag da versão só existe depois do merge.

set -eu

step() { echo "== $*"; }
fail() { echo "FAIL: $*" >&2; exit 1; }

src=$(pwd)
ports=/var/tmp/ports
distdir=/var/tmp/distfiles
port=$ports/sysutils/bectl-gui

pkg install -y git portlint qt-sudo >/dev/null

step "árvore de ports (só Mk, Templates e Keywords)"
rm -rf "$ports"
git clone -q --depth 1 --filter=blob:none --sparse \
	https://github.com/freebsd/freebsd-ports.git "$ports"
git -C "$ports" sparse-checkout set Mk Templates Keywords
mkdir -p "$port" "$distdir"
cp freebsd/Makefile freebsd/pkg-descr "$port/"

mk() { make -C "$port" PORTSDIR="$ports" DISTDIR="$distdir" BATCH=yes "$@"; }

step "distfile a partir do commit em teste"
distfile=$(mk -V DISTFILES:C/:.*//)
prefix=$(basename "$(mk -V WRKSRC)")
echo "$distfile ($prefix/)"
git -c safe.directory="$src" -C "$src" archive --format=tar.gz \
	--prefix="$prefix/" -o "$distdir/$distfile" HEAD
mk makesum
cat "$port/distinfo"

step "portlint"
# -A: todos os testes; FATAL encerra com status diferente de zero. O
# portlint lê a árvore de ports do ambiente, não do make.
PORTSDIR=$ports portlint -A "$port"

step "build, stage-qa e check-plist"
mk stage stage-qa check-plist

step "pacote"
mk package
pkgfile=$(mk -V PKGFILE)
ls -l "$pkgfile"
pkg info -F "$pkgfile" | sed -n '1,/^Description/p'

step "instalação do pacote"
pkg add "$pkgfile"
pkg info -l bectl-gui
[ -x /usr/local/bin/bectl_gui ] || fail "bectl_gui não foi instalado"

# Mesmo critério de tests/freebsd-e2e.sh: precisa continuar de pé até o
# timeout(1) encerrá-lo (status 124).
step "binário instalado (offscreen)"
set +e
QT_QPA_PLATFORM=offscreen timeout 15 /usr/local/bin/bectl_gui >/var/tmp/bectl_gui-pkg.log 2>&1
rc=$?
set -e
cat /var/tmp/bectl_gui-pkg.log
[ "$rc" -eq 124 ] || fail "bectl_gui instalado terminou com status $rc"

pkg delete -y bectl-gui >/dev/null
step "port validado"
