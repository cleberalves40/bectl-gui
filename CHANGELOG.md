# Changelog

Todas as mudanças relevantes do bectl-gui são registradas aqui.
O formato segue o [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/)
e o projeto usa [versionamento semântico](https://semver.org/lang/pt-BR/),
com tags no formato `vX.Y.Z`.

## [0.1.1] - 2026-10-17

### Adicionado
- Homologação nas arquiteturas Tier 1 do FreeBSD (amd64 e aarch64), nas
  releases 14.5 e 15.1, com um job de CI para cada combinação.
- `tests/tst_bectlmanager.cpp` e `tests/freebsd-e2e.sh`: teste de integração do
  `BectlManager` contra o `bectl(8)` real, num pool ZFS de rascunho, mais um
  teste de inicialização da GUI.

### Corrigido
- A versão do projeto passa a ter uma fonte única (`project(... VERSION ...)` no
  `CMakeLists.txt`), exibida na janela "Sobre" e em
  `QApplication::applicationVersion()`. Antes, o CMake e o port declaravam 1.0.0
  e a janela "Sobre" citava apenas "FreeBSD 15.1".
- `freebsd/Makefile`: `DISTVERSIONPREFIX=v`, para que `USE_GITHUB` baixe as tags
  `vX.Y.Z`.

## [0.1.0] - 2026-10-17

### Adicionado
- Primeira versão marcada: listagem, criação (com origem e `-r`), ativação
  (permanente e temporária), renomeação e destruição em lote de Boot
  Environments via `bectl(8)`, com elevação de privilégios por `qt-sudo`.
- Temas, tradução (en_US, pt_BR, es_ES), compilação por CMake e QMake, modelo
  de port do FreeBSD e CI no GitHub Actions (Ubuntu e FreeBSD amd64).

[0.1.1]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/cleberalves40/bectl-gui/releases/tag/v0.1.0
