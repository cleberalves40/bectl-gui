# Arquiteturas suportadas

O bectl-gui é homologado nas arquiteturas **Tier 1** do FreeBSD, conforme a
[página de plataformas](https://www.freebsd.org/platforms/) do projeto: **amd64** e
**aarch64**, nas séries 14.x e 15.x. O i386 deixou de ser suportado no FreeBSD 15, e o
armv7, o powerpc64 e o riscv64 são Tier 2.

Cada combinação tem um job próprio no workflow
[Build & Test](../.github/workflows/build.yml), que roda numa VM do
[vmactions/freebsd-vm](https://github.com/vmactions/freebsd-vm) e executa
[`ci/freebsd-ci.sh`](../ci/freebsd-ci.sh). O script compila a GUI e o teste com o Clang
do sistema e os pacotes Qt6 e depois roda [`tests/freebsd-e2e.sh`](../tests/freebsd-e2e.sh).

## O que o teste verifica

O `tests/freebsd-e2e.sh` roda como root e cria um pool ZFS de rascunho num `md(4)`,
organizado como raiz de boot environments (`<pool>/ROOT/default`, com `bootfs`
definido). Em seguida, executa `tst_bectlmanager` contra esse pool por meio de
`bectl -r <pool>/ROOT`, de modo que os boot environments do sistema nunca são tocados. O
teste cobre:

- leitura da saída real de `bectl list -H` (nome, flags, ponto de montagem, espaço e
  data);
- criação a partir de um BE, de um snapshot (`default@snap1`) e rejeição de nome
  duplicado;
- snapshot sem `@` recusado antes de chamar o `bectl`;
- renomeação;
- ativação permanente (`R`) e temporária (`T`);
- montagem e desmontagem;
- destruição em lote, com o resultado de cada item.

Por fim, verifica que a GUI inicia (`QT_QPA_PLATFORM=offscreen`).

## Resultados (0.1.1)

| Release | Arquitetura | Compilador | Qt6 | Binário | Warnings | Testes |
|:---|:---|:---|:---|:---|:---:|:---:|
| 14.5 | amd64 | Clang 21.1.8 | 6.11.2 | ELF 64-bit x86-64 | 0 | 12/12 ✅ |
| 14.5 | aarch64 | Clang 21.1.8 | 6.11.2 | ELF 64-bit ARM aarch64 | 0 | 12/12 ✅ |
| 15.1 | amd64 | Clang 19.1.7 | 6.11.2 | ELF 64-bit x86-64 | 0 | 12/12 ✅ |
| 15.1 | aarch64 | Clang 19.1.7 | 6.11.2 | ELF 64-bit ARM aarch64 | 0 | 12/12 ✅ |

Nenhuma combinação exigiu mudança de código ou flags específicas. O código não depende
de tamanho de tipos, ordem de bytes nem do sinal de `char`: é Qt puro, e a única
integração com o sistema é a execução do `bectl(8)` por `QProcess`.

Observações:

- O FreeBSD 14.5 traz um Clang mais novo (21.1.8) que o 15.1 (19.1.7). Por isso as duas
  séries continuam na matriz, mesmo sem diferença de código.
- O aarch64 roda emulado (TCG) no runner x86_64, como o vmactions recomenda. Os jobs
  levam entre 10 e 30 minutos, contra cerca de 2 minutos no amd64.
- Releases sem suporte de segurança saem da matriz. A 14.4, por exemplo, tem fim de
  suporte em 31/12/2026, e a 14.5 a substitui.

## Executando localmente

Num FreeBSD amd64 ou aarch64, como root e a partir da raiz do código-fonte:

```sh
sh ci/freebsd-ci.sh                # instala dependências, compila e testa
sh tests/freebsd-e2e.sh build      # só o teste, sobre um build com -DBECTL_GUI_BUILD_TESTS=ON
```

O teste carrega o módulo `zfs` se necessário e usa um arquivo de 1 GB em `/var/tmp`,
removido ao final.
