# Changelog

Todas as mudanças relevantes do bectl-gui são registradas aqui.
O formato segue o [Keep a Changelog](https://keepachangelog.com/pt-BR/1.1.0/)
e o projeto usa [versionamento semântico](https://semver.org/lang/pt-BR/),
com tags no formato `vX.Y.Z`.

## [0.2.0] - 2026-10-17

### Adicionado
- **Criar snapshot** (`Ctrl+Shift+S`): cria `be@nome`, opcionalmente recursivo.
  Para clonar um snapshot, a origem em **Criar** agora aceita `be@snapshot`
  digitado.
- **Montar** (`Ctrl+M`) e **Desmontar** (`Ctrl+U`): monta um BE num diretório
  escolhido ou num diretório temporário criado pelo `bectl`.
- As ações acompanham a seleção: ativar, renomear e montar exigem um único BE;
  desmontar só vale para um BE montado que não seja o BE em uso.
- `tests/tst_mainwindow.cpp`: teste da interface com um `bectl` falso, que roda
  em qualquer sistema.

### Alterado
- As operações rodam numa thread à parte: a janela continua respondendo
  enquanto o `bectl` executa e o `qt-sudo` espera a senha. Enquanto isso, as
  ações que chamam o `bectl`, inclusive pelos atalhos, ficam desativadas, e
  fechar a janela pede para aguardar o fim da operação.
- Filtro, seleção, troca de tema e de idioma usam a última listagem. Antes,
  cada tecla digitada no filtro e cada clique na tabela executavam `bectl list`.

### Corrigido
- Em inglês, a aba **Ajuda** mostrava o texto "Help body" em vez do conteúdo.
  A ajuda agora descreve as novas operações nos três idiomas.

## [0.1.2] - 2026-10-17

### Corrigido
- Comandos privilegiados não têm mais o limite de 60 s. O limite incluía o
  tempo de digitar a senha no `qt-sudo`, então quem demorasse tinha a operação
  cancelada; operações longas, como `destroy`, também podiam ser interrompidas.
  O limite continua valendo para as leituras sem privilégio.
- Nomes de Boot Environment e de snapshot são validados antes de chegar ao
  `bectl(8)`, com as mesmas regras do `libbe` (letras, dígitos e `_-.:`). Um
  nome iniciado por `-` era passado como argumento ao `bectl` executado como
  root e interpretado como opção. Pontos de montagem precisam ser absolutos.
- As mensagens de erro do `BectlManager` eram fixas em português; agora seguem
  o idioma da interface (en_US, pt_BR, es_ES).

### Adicionado
- `tests/tst_validation.cpp`: teste unitário das validações e mensagens, que
  roda em qualquer sistema, inclusive no job Ubuntu do CI.

## [0.1.1] - 2026-10-17

### Adicionado
- Homologação nas arquiteturas Tier 1 do FreeBSD (amd64 e aarch64), nas
  releases 14.5 e 15.1, com um job de CI para cada combinação. As quatro
  compilam sem warnings e passam no teste de integração, sem nenhum ajuste de
  código ou de flags.
- `tests/tst_bectlmanager.cpp` e `tests/freebsd-e2e.sh`: teste de integração do
  `BectlManager` contra o `bectl(8)` real, num pool ZFS de rascunho, mais um
  teste de inicialização da GUI.
- `docs/ARCHITECTURES.md`: resultados por arquitetura e release.

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

[0.2.0]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.2...v0.2.0
[0.1.2]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.1...v0.1.2
[0.1.1]: https://github.com/cleberalves40/bectl-gui/compare/v0.1.0...v0.1.1
[0.1.0]: https://github.com/cleberalves40/bectl-gui/releases/tag/v0.1.0
