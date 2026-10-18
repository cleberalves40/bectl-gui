# Versões e envio do port

## Versionamento

O projeto segue o [versionamento semântico](https://semver.org/lang/pt-BR/), com tags
`vX.Y.Z`:

- **patch** (`v1.0.1`): correções, sem funcionalidades novas;
- **minor** (`v1.1.0`): funcionalidades novas, compatíveis;
- **major** (`v2.0.0`): mudanças incompatíveis.

A versão fica em três arquivos, e o CI reprova se eles divergirem ou se o `CHANGELOG.md`
não tiver a entrada correspondente:

| Arquivo | Campo |
|:---|:---|
| `CMakeLists.txt` | `project(bectl_gui VERSION X.Y.Z ...)`: a fonte da versão exibida no programa |
| `freebsd/Makefile` | `DISTVERSION=` |
| `bectl_gui.pro` | `DEFINES += BECTL_GUI_VERSION=...` |

## Publicando uma versão

1. Num branch `release-X.Y.Z`, altere os três arquivos e registre as mudanças no
   `CHANGELOG.md`, no formato [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/),
   com o link de comparação no fim do arquivo.
2. Abra o pull request. O CI precisa passar no Ubuntu (com os testes também sob ASan e
   UBSan) e nos quatro jobs FreeBSD Tier 1, que incluem a validação do port.
3. Faça o merge com um *merge commit*.
4. Crie a tag anotada no commit de merge e publique a release com o texto do CHANGELOG:

   ```sh
   git fetch origin
   git tag -a vX.Y.Z -m "bectl-gui vX.Y.Z" origin/main
   git push origin vX.Y.Z
   ```

## Enviando o port ao FreeBSD

O port fica em `freebsd/` e é validado a cada pull request (veja
[ARCHITECTURES.md](ARCHITECTURES.md)). Para propô-lo na árvore oficial, como
`sysutils/bectl-gui`, depois de publicar a tag:

1. Numa máquina FreeBSD com a árvore de ports atualizada, copie `freebsd/Makefile` e
   `freebsd/pkg-descr` para `/usr/ports/sysutils/bectl-gui/` e gere o `distinfo` a
   partir do tarball da tag publicada:

   ```sh
   cd /usr/ports/sysutils/bectl-gui
   make makesum
   ```

2. Teste num ambiente limpo, nas arquiteturas que puder (o CI não usa o `poudriere`):

   ```sh
   portlint -AC
   poudriere testport -j <jail> -o sysutils/bectl-gui
   ```

3. Adicione `bectl-gui` ao `SUBDIR` de `sysutils/Makefile` e gere o patch com
   `git format-patch`, a partir da árvore de ports em git.
4. Abra um relatório em <https://bugs.freebsd.org/> (produto *Ports & Packages*,
   componente *Individual Port(s)*), com o título `[NEW PORT] sysutils/bectl-gui: Qt6
   graphical front-end for bectl(8)`, e anexe o patch e o log do `poudriere testport`.

O `MAINTAINER` do port é `cleber@bsd.com.br`, que recebe as notificações do relatório e
das atualizações futuras.
