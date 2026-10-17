# bectl GUI (C++/Qt6) – Gerenciador de Boot Environments (FreeBSD)

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![Framework](https://img.shields.io/badge/Framework-Qt6-green.svg)](https://www.qt.io/)
[![Platform](https://img.shields.io/badge/Platform-FreeBSD-red.svg)](https://www.freebsd.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Build & Test](https://github.com/cleberalves40/bectl-gui/actions/workflows/build.yml/badge.svg)](https://github.com/cleberalves40/bectl-gui/actions/workflows/build.yml)

Interface gráfica nativa em **C++ / Qt6** para o utilitário [`bectl(8)`](https://man.freebsd.org/cgi/man.cgi?bectl(8)) do FreeBSD, permitindo gerenciar Boot Environments ZFS de forma rápida, segura e intuitiva.

Desenvolvida e otimizada para **FreeBSD 14/15** com o compilador Clang/LLVM nativo.

---

## 🏛️ Arquitetura

```text
bectl-gui/
├── CMakeLists.txt           # Build system principal (CMake)
├── bectl_gui.pro            # Build system alternativo (QMake)
├── bectl_gui.desktop        # Entrada para o menu de aplicativos desktop
├── freebsd/                 # Modelo de porta para o FreeBSD Ports
│   ├── Makefile
│   └── pkg-descr
├── .github/workflows/       # CI/CD automatizado via GitHub Actions
│   └── build.yml
├── README.md
├── LICENSE
└── src/
    ├── main.cpp             # Ponto de entrada (QApplication & i18n)
    ├── logic/               # Camada de lógica e integração de sistema
    │   ├── bectlmanager.h   # Encapsulamento de comandos bectl + qt-sudo
    │   └── bectlmanager.cpp
    ├── gui/                 # Interface gráfica Qt6 (estilo OctoPKG)
    │   ├── mainwindow.h
    │   ├── mainwindow.cpp
    │   ├── themes.h         # Paletas (Catppuccin, Nord, Dracula, etc.)
    │   └── themes.cpp
    └── i18n/                # Internacionalização (en_US, pt_BR, es_ES)
        ├── i18n.h
        └── i18n.cpp
```

| Componente | Localização | Responsabilidade |
|:---|:---|:---|
| **Lógica** | `src/logic/bectlmanager.*` | Execução segura dos comandos `bectl` via `QProcess` |
| **Interface GUI** | `src/gui/mainwindow.*` | Janela Qt6, tabela com indicadores de estado e diálogos |
| **Temas** | `src/gui/themes.*` | Folhas de estilo QSS para mais de 10 temas |
| **i18n** | `src/i18n/i18n.*` | Tradução dinâmica (en_US, pt_BR, es_ES) |
| **Entrada** | `src/main.cpp` | Inicialização da `QApplication` e configuração regional |

### Elevação de privilégios com qt-sudo

Todas as operações de escrita que exigem permissão de superusuário são executadas de forma transparente via `qt-sudo`:

```text
qt-sudo bectl create nome-do-be
qt-sudo bectl activate nome-do-be
qt-sudo bectl destroy -F nome-do-be
```

---

## ✨ Funcionalidades

- **Listagem completa**: Lista todos os Boot Environments (`bectl list -H`) com uso de espaço e ponto de montagem.
- **Destaque visual**:
  - 🟢 **Verde**: Ativo agora (`N` - Now).
  - 🟡 **Amarelo**: Ativo no próximo boot (`R` - Reboot).
- **Criação flexível**: Cria novos BEs com suporte a origem customizada e recursividade (`-r`).
- **Ativação**: Suporte à ativação padrão ou temporária para o próximo boot (`-t`).
- **Renomeação**: Renomeia BEs existentes com validação prévia.
- **Snapshots**: Cria snapshots (`be@nome`) e clona BEs a partir deles.
- **Montagem**: Monta um BE num diretório (ou num diretório temporário criado pelo `bectl`) para inspecionar seus arquivos, e desmonta.
- **Interface responsiva**: As operações rodam em segundo plano; a janela continua respondendo enquanto o `bectl` executa e o `qt-sudo` pede a senha.
- **Destruição em lote**: Selecione múltiplos BEs com `Ctrl+Clique` ou `Shift+Clique`.
- **Proteção ativa**: Impede acidentes bloqueando a exclusão do Boot Environment ativo no momento.
- **Multilíngue**: Suporte a Inglês, Português (Brasil) e Espanhol com detecção automática do sistema ou seleção manual.
- **Temas integrados**: Paleta OctoPKG padrão, Catppuccin (Frappé, Latte, Mocha, Macchiato), Solarized, Selenized, Dracula, Tokyo Night, Nord, Gruvbox e One Dark.

---

## 📦 Requisitos

No **FreeBSD**:

```bash
# Ferramentas de compilação
pkg install cmake ninja

# Qt6
pkg install qt6-base qt6-tools

# Elevação de privilégios com interface gráfica
pkg install qt-sudo
```

---

## 🛠️ Compilação e Instalação

### Opção 1: Com CMake + Ninja (Recomendado)

```bash
# 1. Alinhar timestamps (evita aviso de 'Clock skew' em montagens ZFS/NFS)
sh fix-timestamps.sh

# 2. Configurar o projeto
cmake -B build -S . -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_CXX_COMPILER=clang++

# 3. Compilar
cmake --build build -j$(sysctl -n hw.ncpu)

# 4. Executar
./build/bectl_gui

# 5. (Opcional) Instalar no sistema (/usr/local/bin e share/applications)
cmake --install build --prefix /usr/local
```

### Opção 2: Com Make tradicional

```bash
sh fix-timestamps.sh
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++
cmake --build build -j$(sysctl -n hw.ncpu)
./build/bectl_gui
```

### Opção 3: Com QMake

```bash
mkdir -p build && cd build
qmake6 ../bectl_gui.pro
make -j$(sysctl -n hw.ncpu)
./bectl_gui
```

### Testes

Num FreeBSD, como root, o teste de integração roda contra o `bectl(8)` real num pool ZFS
de rascunho, sem tocar nos boot environments do sistema:

```bash
cmake -B build -S . -G Ninja -DBECTL_GUI_BUILD_TESTS=ON
cmake --build build
sh tests/freebsd-e2e.sh build
```

---

## 🖥️ Plataformas suportadas

Homologado nas arquiteturas **Tier 1** do FreeBSD, **amd64** e **aarch64**, nas releases
**14.5** e **15.1**. Cada combinação tem um job no CI que compila e executa o teste de
integração. Os resultados e detalhes estão em [docs/ARCHITECTURES.md](docs/ARCHITECTURES.md),
e o histórico de versões no [CHANGELOG.md](CHANGELOG.md).

---

## ⌨️ Atalhos de Teclado

| Atalho | Ação |
|:---|:---|
| <kbd>F5</kbd> ou <kbd>Ctrl+R</kbd> | Atualizar lista de BEs |
| <kbd>Ctrl+N</kbd> | Criar novo Boot Environment |
| <kbd>Ctrl+Enter</kbd> | Ativar BE selecionado |
| <kbd>Ctrl+T</kbd> | Ativar temporariamente (próximo boot) |
| <kbd>F2</kbd> | Renomear BE |
| <kbd>Ctrl+Shift+S</kbd> | Criar snapshot |
| <kbd>Ctrl+M</kbd> | Montar BE selecionado |
| <kbd>Ctrl+U</kbd> | Desmontar BE selecionado |
| <kbd>Delete</kbd> | Destruir BE(s) selecionado(s) |
| <kbd>Ctrl+F</kbd> | Focar no campo de busca/filtro |
| <kbd>Ctrl+Shift+T</kbd> | Alternar para o próximo tema |
| <kbd>Ctrl+1 / 2 / 3</kbd> | Alternar abas (Info / Saída / Ajuda) |
| <kbd>Ctrl+Q</kbd> | Sair |

---

## 🛡️ Segurança

- O aplicativo **nunca é executado como root**. Toda a interface gráfica roda com as credenciais do usuário logado.
- Apenas as chamadas de gravação do `bectl` invocam o `qt-sudo` pontualmente.
- A exclusão de BEs requer confirmação explícita na interface gráfica e bloqueia o BE ativo.

---

## 📄 Licença

Distribuído sob a licença [MIT](LICENSE).
