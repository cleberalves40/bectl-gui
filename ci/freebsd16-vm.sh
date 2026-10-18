#!/bin/bash
#
# Homologação no FreeBSD 16.0-CURRENT: nenhuma action de CI oferece essa
# versão, então este script dá boot na imagem oficial de snapshot no QEMU e
# executa nela os mesmos testes dos jobs 14.5/15.1 (ci/freebsd-ci.sh e
# ci/freebsd-port.sh), sobre o código da tag indicada. Roda no runner Linux.
#
# uso: ci/freebsd16-vm.sh <amd64|aarch64> <tag>

set -eu

arch=$1
tag=$2
work=${RUNNER_TEMP:-/tmp}/fbsd16-$arch
mkdir -p "$work/serve"

case $arch in
amd64)   prefix=FreeBSD-16.0-CURRENT-amd64-BASIC-CLOUDINIT- ;;
aarch64) prefix=FreeBSD-16.0-CURRENT-arm64-aarch64-BASIC-CLOUDINIT- ;;
*) echo "arquitetura desconhecida: $arch" >&2; exit 2 ;;
esac
base=https://download.freebsd.org/snapshots/VM-IMAGES/16.0-CURRENT/$arch

# O CHECKSUM de Latest/ lista o snapshot pelo nome completo
# (<prefixo><data>-<commit>-<revisão>-ufs.qcow2.xz), que fica no diretório
# da data; o nome identifica exatamente o que foi testado.
line=$(curl -fsSL "$base/Latest/CHECKSUM.SHA256" | grep -E "^SHA256 \(${prefix}[0-9]+-[0-9a-f]+-[0-9]+-ufs\.qcow2\.xz\)")
xzfile=$(echo "$line" | sed -E 's/^SHA256 \((.*)\) = .*$/\1/')
sum=$(echo "$line" | sed -E 's/^.* = ([0-9a-f]+)$/\1/')
date=$(echo "$xzfile" | sed -E "s/^${prefix}([0-9]+)-.*/\1/")
img=${xzfile%.xz}
echo "== snapshot: $xzfile"
curl -fsSL -o "$work/$xzfile" "$base/$date/$xzfile"
echo "$sum  $work/$xzfile" | sha256sum -c
xz -d -f "$work/$xzfile"
qemu-img resize -q "$work/$img" +20G

# Código da tag, servido à VM pela rede de usuário do QEMU (host = 10.0.2.2).
git archive --format=tar.gz -o "$work/serve/src.tgz" "$tag"

# Configuração inicial (nuageinit, disco "cidata"): um script executado como
# root no fim do boot. A saída volta ao runner por nc, e a VM se desliga.
cat >"$work/user-data" <<'EOF'
#!/bin/sh
log=/var/log/bectl-ci.log
{
	set -x
	uname -a
	for i in $(seq 1 60); do fetch -q -o /dev/null http://10.0.2.2:8000/src.tgz && break; sleep 2; done
	fetch -o /tmp/src.tgz http://10.0.2.2:8000/src.tgz
	mkdir -p /tmp/src && tar -xzf /tmp/src.tgz -C /tmp/src
	command -v pkg >/dev/null || env ASSUME_ALWAYS_YES=yes pkg bootstrap -f
	set +x
	cd /tmp/src && env ASSUME_ALWAYS_YES=yes sh ci/freebsd-ci.sh && sh ci/freebsd-port.sh
	echo "BECTL-CI-RESULT=$?"
} >"$log" 2>&1
nc -N 10.0.2.2 9000 <"$log"
shutdown -p now
EOF
printf 'instance-id: bectl-ci\nlocal-hostname: bectl-ci\n' >"$work/meta-data"
genisoimage -quiet -output "$work/seed.iso" -volid cidata -joliet -rock "$work/user-data" "$work/meta-data"

python3 -m http.server 8000 --bind 127.0.0.1 --directory "$work/serve" >/dev/null 2>&1 &
http=$!
nc -l 127.0.0.1 9000 >"$work/result.log" &
listener=$!
trap 'kill $http $listener 2>/dev/null || true' EXIT

# O nuageinit acha a configuração pelo rótulo "cidata" do sistema de arquivos;
# o ISO entra como um disco virtio comum.
common="-m 6G -smp 4 -display none -serial file:$work/console.log -monitor none
	-drive file=$work/$img,if=virtio,format=qcow2
	-drive file=$work/seed.iso,if=virtio,format=raw,readonly=on
	-nic user,model=virtio-net-pci"
echo "== boot do FreeBSD 16.0-CURRENT/$arch"
case $arch in
amd64)
	# shellcheck disable=SC2086
	timeout 100m qemu-system-x86_64 -accel kvm -accel tcg -cpu max $common || echo "== qemu terminou com status $?" ;;
aarch64)
	# shellcheck disable=SC2086
	timeout 150m qemu-system-aarch64 -M virt -cpu max -bios /usr/share/qemu-efi-aarch64/QEMU_EFI.fd $common || echo "== qemu terminou com status $?" ;;
esac

# O resultado chega antes do desligamento; se a VM nunca conectou, o listener
# ainda espera e é encerrado aqui.
sleep 2
kill $listener 2>/dev/null || true
echo "== resultado"
cat "$work/result.log"
grep -q '^BECTL-CI-RESULT=0$' "$work/result.log"
