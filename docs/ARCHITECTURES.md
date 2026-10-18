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

## O que o CI verifica

Em cada combinação, nesta ordem:

1. **Build** com o Clang do sistema e os pacotes Qt6, sem warnings.
2. **`tst_validation`**: validação de nomes e caminhos, argumentos que chegam ao
   `bectl`, status de saída, tradução das mensagens e leitura de `bectl list -s`, com um
   `bectl` falso.
3. **`tst_mainwindow`**: a janela com um `bectl` falso (plataforma offscreen). Verifica que
   ela responde durante a execução, que filtro e seleção usam a lista em cache, que as
   ações e a aba Snapshots acompanham a seleção e que uma operação em andamento bloqueia
   outras e o fechamento.
4. **`tst_bectlmanager`**: integração contra o `bectl(8)` real. O
   [`tests/freebsd-e2e.sh`](../tests/freebsd-e2e.sh) cria um pool ZFS de rascunho num
   `md(4)`, organizado como raiz de boot environments (`<pool>/ROOT/default`, com
   `bootfs`), e executa o teste por meio de `bectl -r <pool>/ROOT`, sem tocar nos boot
   environments do sistema. Cobre listagem de BEs e de snapshots, criação a partir de BE
   e de snapshot, duplicata, renomeação, ativação permanente e temporária, montagem,
   destruição de snapshot e destruição em lote.
5. **Inicialização da GUI** (offscreen).
6. **Port** ([`ci/freebsd-port.sh`](../ci/freebsd-port.sh)), com o framework de ports
   numa árvore esparsa (só `Mk/`, `Templates/`, `Keywords/` e as dependências diretas):
   `portlint`, build, `stage-qa` com `DEVELOPER=yes`, `check-plist`, geração do pacote,
   instalação com `pkg add` e inicialização do binário instalado. O distfile é gerado do
   commit em teste, com o nome e o prefixo do `USE_GITHUB`, porque a tag da versão só
   existe depois do merge.

O port não é construído com o `poudriere`. Ele exigiria uma jail e a compilação das
dependências, caro demais por job, principalmente no aarch64 emulado. A validação acima
usa o mesmo framework, com as dependências instaladas por `pkg`.

## Resultados (1.0.0)

| Release | Arquitetura | Compilador | Qt6 | Warnings | Testes (validação + interface + integração) | Port |
|:---|:---|:---|:---|:---:|:---:|:---|
| 14.5 | amd64 | Clang 21.1.8 | 6.11.2 | 0 | 22 + 8 + 14 ✅ | ✅ `FreeBSD:14:amd64` |
| 14.5 | aarch64 | Clang 21.1.8 | 6.11.2 | 0 | 22 + 8 + 14 ✅ | ✅ `FreeBSD:14:aarch64` |
| 15.1 | amd64 | Clang 19.1.7 | 6.11.2 | 0 | 22 + 8 + 14 ✅ | ✅ `FreeBSD:15:amd64` |
| 15.1 | aarch64 | Clang 19.1.7 | 6.11.2 | 0 | 22 + 8 + 14 ✅ | ✅ `FreeBSD:15:aarch64` |

Nenhuma combinação exigiu mudança de código ou flags específicas. O código não depende
de tamanho de tipos, ordem de bytes nem do sinal de `char`: é Qt puro, e a única
integração com o sistema é a execução do `bectl(8)` por `QProcess`.

No job Ubuntu, os mesmos testes (exceto o de integração, que precisa do `bectl`) rodam
também com AddressSanitizer e UBSan.

Problemas encontrados pela validação do port, ambos corrigidos na 0.2.1:

- O binário liga com `libGLX` e `libOpenGL` (via Qt6 Gui), não declaradas no port
  (erro do `stage-qa`). Resolvido com `USE_GL=gl opengl`.
- `USE_QT=tools:build` declarava o `qt6-tools`, que o build não usa.

O `portlint` termina sem erros fatais, com dois avisos que não se aplicam: o de
`MimeType` (o `.desktop` não declara tipos MIME) e a sugestão de `DEVELOPER=yes`, que o
CI já usa.

Observações:

- O FreeBSD 14.5 traz um Clang mais novo (21.1.8) que o 15.1 (19.1.7). Por isso as duas
  séries continuam na matriz, mesmo sem diferença de código.
- O aarch64 roda emulado (TCG) no runner x86_64, como o vmactions recomenda. Os jobs
  levam entre 25 e 45 minutos, contra cerca de 3 minutos no amd64.
- Releases sem suporte de segurança saem da matriz. A 14.4, por exemplo, tem fim de
  suporte em 31/12/2026, e a 14.5 a substitui.

## Executando localmente

Num FreeBSD amd64 ou aarch64, como root e a partir da raiz do código-fonte:

```sh
sh ci/freebsd-ci.sh                # instala dependências, compila e testa
sh tests/freebsd-e2e.sh build      # só os testes, sobre um build com -DBECTL_GUI_BUILD_TESTS=ON
sh ci/freebsd-port.sh              # valida o port (depois de ci/freebsd-ci.sh)
```

O teste carrega o módulo `zfs` se necessário e usa um arquivo de 1 GB em `/var/tmp`,
removido ao final.
