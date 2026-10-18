#include "i18n.h"
#include <QLocale>
#include <QHash>
#include <QMutex>

namespace I18n {
namespace {
QString g_locale = QStringLiteral("en_US");
QHash<QString, QHash<QString, QString>> g_cats;
bool g_init = false;

void ensureInit()
{
    if (g_init) return;
    g_init = true;
    {
        QHash<QString, QString> c;
        c.insert(QStringLiteral("&File"), QStringLiteral("&Arquivo"));
        c.insert(QStringLiteral("&Actions"), QStringLiteral("&Ações"));
        c.insert(QStringLiteral("&Theme"), QStringLiteral("&Tema"));
        c.insert(QStringLiteral("&Help"), QStringLiteral("&Ajuda"));
        c.insert(QStringLiteral("&Quit"), QStringLiteral("&Sair"));
        c.insert(QStringLiteral("&Refresh"), QStringLiteral("&Atualizar"));
        c.insert(QStringLiteral("&Create…"), QStringLiteral("&Criar…"));
        c.insert(QStringLiteral("A&ctivate"), QStringLiteral("A&tivar"));
        c.insert(QStringLiteral("Activate (&temporary)"), QStringLiteral("Ativar (&temporário)"));
        c.insert(QStringLiteral("&Rename…"), QStringLiteral("&Renomear…"));
        c.insert(QStringLiteral("&Destroy selected"), QStringLiteral("&Destruir selecionado(s)"));
        c.insert(QStringLiteral("About bectl…"), QStringLiteral("Sobre bectl…"));
        c.insert(QStringLiteral("Keyboard shortcuts"), QStringLiteral("Atalhos de teclado"));
        c.insert(QStringLiteral("Shortcuts"), QStringLiteral("Atalhos"));
        c.insert(QStringLiteral("Language"), QStringLiteral("Idioma"));
        c.insert(QStringLiteral("Ready."), QStringLiteral("Pronto."));
        c.insert(QStringLiteral("0 BE(s)"), QStringLiteral("0 BE(s)"));
        c.insert(QStringLiteral("%1 BE(s)"), QStringLiteral("%1 BE(s)"));
        c.insert(QStringLiteral("Filter by name…"), QStringLiteral("Filtrar por nome…"));
        c.insert(QStringLiteral("Boot Environment"), QStringLiteral("Boot Environment"));
        c.insert(QStringLiteral("Active"), QStringLiteral("Ativo"));
        c.insert(QStringLiteral("Mountpoint"), QStringLiteral("Montagem"));
        c.insert(QStringLiteral("Space"), QStringLiteral("Espaço"));
        c.insert(QStringLiteral("Created"), QStringLiteral("Criado em"));
        c.insert(QStringLiteral("Information"), QStringLiteral("Informação"));
        c.insert(QStringLiteral("Output"), QStringLiteral("Saída"));
        c.insert(QStringLiteral("Help"), QStringLiteral("Ajuda"));
        c.insert(QStringLiteral("Select a Boot Environment to see details."), QStringLiteral("Selecione um Boot Environment para ver detalhes."));
        c.insert(QStringLiteral("Command output (bectl / qt-sudo)…"), QStringLiteral("Saída das operações (bectl / qt-sudo)…"));
        c.insert(QStringLiteral("Listing Boot Environments…"), QStringLiteral("Listando Boot Environments…"));
        c.insert(QStringLiteral("Error listing."), QStringLiteral("Erro ao listar."));
        c.insert(QStringLiteral("Error"), QStringLiteral("Erro"));
        c.insert(QStringLiteral("List updated (%1 BE(s))."), QStringLiteral("Lista atualizada (%1 BE(s))."));
        c.insert(QStringLiteral("ERROR: %1"), QStringLiteral("ERRO: %1"));
        c.insert(QStringLiteral("Active now (N)"), QStringLiteral("Ativo agora (N)"));
        c.insert(QStringLiteral("Active on reboot (R)"), QStringLiteral("Ativo no próximo boot (R)"));
        c.insert(QStringLiteral("Inactive"), QStringLiteral("Inativo"));
        c.insert(QStringLiteral("Active now"), QStringLiteral("Ativo agora"));
        c.insert(QStringLiteral("Active on next boot"), QStringLiteral("Ativo no próximo boot"));
        c.insert(QStringLiteral("State"), QStringLiteral("Estado"));
        c.insert(QStringLiteral("Create Boot Environment"), QStringLiteral("Criar Boot Environment"));
        c.insert(QStringLiteral("Name:"), QStringLiteral("Nome:"));
        c.insert(QStringLiteral("Clone from:"), QStringLiteral("Clonar de:"));
        c.insert(QStringLiteral("(Current Boot Environment)"), QStringLiteral("(Boot Environment atual)"));
        c.insert(QStringLiteral("Create recursively (-r)"), QStringLiteral("Criar de forma recursiva (-r)"));
        c.insert(QStringLiteral("Name"), QStringLiteral("Nome"));
        c.insert(QStringLiteral("Please enter a name."), QStringLiteral("Informe um nome."));
        c.insert(QStringLiteral("Creating… (qt-sudo may ask for a password)"), QStringLiteral("Criando… (qt-sudo pode solicitar senha)"));
        c.insert(QStringLiteral("Success"), QStringLiteral("Sucesso"));
        c.insert(QStringLiteral("Boot Environment '%1' created."), QStringLiteral("Boot Environment '%1' criado."));
        c.insert(QStringLiteral("OK: '%1' created."), QStringLiteral("OK: '%1' criado."));
        c.insert(QStringLiteral("Selection"), QStringLiteral("Seleção"));
        c.insert(QStringLiteral("Select exactly one Boot Environment."), QStringLiteral("Selecione exatamente um Boot Environment."));
        c.insert(QStringLiteral("Activate"), QStringLiteral("Ativar"));
        c.insert(QStringLiteral("Activate '%1' permanently?"), QStringLiteral("Ativar permanentemente '%1'?"));
        c.insert(QStringLiteral("Activate (temporary)"), QStringLiteral("Ativar (temporário)"));
        c.insert(QStringLiteral("Activate '%1' temporarily?"), QStringLiteral("Ativar temporariamente '%1'?"));
        c.insert(QStringLiteral("OK: activated."), QStringLiteral("OK: ativado."));
        c.insert(QStringLiteral("OK: activated (temporary)."), QStringLiteral("OK: ativado (temporário)."));
        c.insert(QStringLiteral("Rename"), QStringLiteral("Renomear"));
        c.insert(QStringLiteral("New name:"), QStringLiteral("Novo nome:"));
        c.insert(QStringLiteral("OK: renamed."), QStringLiteral("OK: renomeado."));
        c.insert(QStringLiteral("Select one or more Boot Environments."), QStringLiteral("Selecione um ou mais Boot Environments."));
        c.insert(QStringLiteral("Active BE"), QStringLiteral("BE ativo"));
        c.insert(QStringLiteral("Skipped (active):\n• %1"), QStringLiteral("Ignorados (ativos):\n• %1"));
        c.insert(QStringLiteral("Confirm destroy"), QStringLiteral("Confirmar destruição"));
        c.insert(QStringLiteral("WARNING: destroying '%1' is irreversible.\nContinue?"), QStringLiteral("ATENÇÃO: destruir '%1' é irreversível.\nContinuar?"));
        c.insert(QStringLiteral("WARNING: destroying %1 Boot Environments is irreversible:\n• %2\nContinue?"), QStringLiteral("ATENÇÃO: destruir %1 Boot Environments é irreversível:\n• %2\nContinuar?"));
        c.insert(QStringLiteral("Destroyed (%1):\n• %2\n"), QStringLiteral("Destruídos (%1):\n• %2\n"));
        c.insert(QStringLiteral("Failures (%1):\n• %2"), QStringLiteral("Falhas (%1):\n• %2"));
        c.insert(QStringLiteral("Result"), QStringLiteral("Resultado"));
        c.insert(QStringLiteral("Done."), QStringLiteral("Concluído."));
        c.insert(QStringLiteral("Theme: %1"), QStringLiteral("Tema: %1"));
        c.insert(QStringLiteral("About bectl"), QStringLiteral("Sobre bectl"));
        c.insert(QStringLiteral("bectl — Boot Environment Manager"), QStringLiteral("bectl — Gerenciador de Boot Environments"));
        c.insert(QStringLiteral("Graphical manager for FreeBSD Boot Environments."), QStringLiteral("Gerenciador gráfico de Boot Environments (FreeBSD)."));
        c.insert(QStringLiteral("UI inspired by OctoPKG (main list + Information / Output / Help tabs)."), QStringLiteral("Interface inspirada no OctoPKG (lista principal + abas Informação / Saída / Ajuda)."));
        c.insert(QStringLiteral("Logic: bectl · privilege: qt-sudo"), QStringLiteral("Lógica: bectl · elevação: qt-sudo"));
        c.insert(QStringLiteral("Main"), QStringLiteral("Principal"));
        c.insert(QStringLiteral("English (US)"), QStringLiteral("Inglês (EUA)"));
        c.insert(QStringLiteral("Portuguese (Brazil)"), QStringLiteral("Português (Brasil)"));
        c.insert(QStringLiteral("Spanish (Spain)"), QStringLiteral("Espanhol (Espanha)"));
        c.insert(QStringLiteral("Help body"), QStringLiteral("<h3>bectl — Gerenciador de Boot Environments</h3><p>Interface inspirada no <b>OctoPKG</b> (lista + abas de detalhe).</p><ul><li><b>Atualizar</b> — bectl list -H</li><li><b>Criar</b> — novo BE, opcionalmente clonado de outro BE ou de um snapshot (digite <i>be@snapshot</i>)</li><li>Aba <b>Snapshots</b> — snapshots do BE selecionado: criar, clonar num novo BE, destruir</li><li><b>Ativar</b> — permanente ou temporário (-t)</li><li><b>Montar / Desmontar</b> — inspecionar os arquivos de um BE num diretório</li><li><b>Renomear / Destruir</b> — com confirmação; BE ativo protegido</li></ul><p>Operações privilegiadas usam <b>qt-sudo</b>. A janela continua respondendo enquanto elas rodam.</p><p>Atalhos: F5 atualizar · Ctrl+Q sair · Ajuda › Atalhos de teclado</p>"));
        // Mensagens do BectlManager
        c.insert(QStringLiteral("'%1' not found. Install the security/qt-sudo package (pkg install qt-sudo)."), QStringLiteral("'%1' não encontrado. Instale o pacote security/qt-sudo (pkg install qt-sudo)."));
        c.insert(QStringLiteral("Failed to start '%1': %2"), QStringLiteral("Falha ao iniciar '%1': %2"));
        c.insert(QStringLiteral("Command timed out (%1 ms): %2"), QStringLiteral("Comando excedeu o tempo limite (%1 ms): %2"));
        c.insert(QStringLiteral("Process crashed: %1"), QStringLiteral("Processo terminou com crash: %1"));
        c.insert(QStringLiteral("Command failed with exit code %1"), QStringLiteral("Comando falhou com código %1"));
        c.insert(QStringLiteral("Snapshot name must contain '@' (e.g. default@before-upgrade)"), QStringLiteral("O nome do snapshot deve conter '@' (ex: default@antes-upgrade)"));
        c.insert(QStringLiteral("Invalid boot environment name: %1"), QStringLiteral("Nome de Boot Environment inválido: %1"));
        c.insert(QStringLiteral("Invalid snapshot name: %1"), QStringLiteral("Nome de snapshot inválido: %1"));
        c.insert(QStringLiteral("Mount point must be an absolute path: %1"), QStringLiteral("O ponto de montagem deve ser um caminho absoluto: %1"));
        c.insert(QStringLiteral("%1: cannot destroy the currently active BE"), QStringLiteral("%1: não é possível destruir o BE ativo no momento"));
        // v0.2.0: operações assíncronas, montagem e snapshots
        c.insert(QStringLiteral("&Mount…"), QStringLiteral("&Montar…"));
        c.insert(QStringLiteral("&Unmount"), QStringLiteral("&Desmontar"));
        c.insert(QStringLiteral("Create &snapshot…"), QStringLiteral("Criar &snapshot…"));
        c.insert(QStringLiteral("Create snapshot"), QStringLiteral("Criar snapshot"));
        c.insert(QStringLiteral("Boot Environment:"), QStringLiteral("Boot Environment:"));
        c.insert(QStringLiteral("Snapshot name:"), QStringLiteral("Nome do snapshot:"));
        c.insert(QStringLiteral("Mount Boot Environment"), QStringLiteral("Montar Boot Environment"));
        c.insert(QStringLiteral("Mount point:"), QStringLiteral("Ponto de montagem:"));
        c.insert(QStringLiteral("Browse…"), QStringLiteral("Procurar…"));
        c.insert(QStringLiteral("Leave empty to let bectl create a temporary directory."), QStringLiteral("Deixe vazio para o bectl criar um diretório temporário."));
        c.insert(QStringLiteral("Activating… (qt-sudo may ask for a password)"), QStringLiteral("Ativando… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("Renaming… (qt-sudo may ask for a password)"), QStringLiteral("Renomeando… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("Destroying… (qt-sudo may ask for a password)"), QStringLiteral("Destruindo… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("Mounting… (qt-sudo may ask for a password)"), QStringLiteral("Montando… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("Unmounting… (qt-sudo may ask for a password)"), QStringLiteral("Desmontando… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("Creating snapshot… (qt-sudo may ask for a password)"), QStringLiteral("Criando snapshot… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("OK: '%1' mounted."), QStringLiteral("OK: '%1' montado."));
        c.insert(QStringLiteral("OK: '%1' unmounted."), QStringLiteral("OK: '%1' desmontado."));
        c.insert(QStringLiteral("Wait for the current operation to finish."), QStringLiteral("Aguarde o fim da operação em andamento."));
        // v0.3.0: aba Snapshots
        c.insert(QStringLiteral("C&lone snapshot…"), QStringLiteral("C&lonar snapshot…"));
        c.insert(QStringLiteral("Destroy s&napshot"), QStringLiteral("Destruir s&napshot"));
        c.insert(QStringLiteral("Snapshots"), QStringLiteral("Snapshots"));
        c.insert(QStringLiteral("Snapshot"), QStringLiteral("Snapshot"));
        c.insert(QStringLiteral("Select a snapshot."), QStringLiteral("Selecione um snapshot."));
        c.insert(QStringLiteral("Clone snapshot"), QStringLiteral("Clonar snapshot"));
        c.insert(QStringLiteral("New Boot Environment name:"), QStringLiteral("Nome do novo Boot Environment:"));
        c.insert(QStringLiteral("Cloning… (qt-sudo may ask for a password)"), QStringLiteral("Clonando… (o qt-sudo pode pedir a senha)"));
        c.insert(QStringLiteral("OK: '%1' created from '%2'."), QStringLiteral("OK: '%1' criado a partir de '%2'."));
        c.insert(QStringLiteral("OK: snapshot '%1' created."), QStringLiteral("OK: snapshot '%1' criado."));
        c.insert(QStringLiteral("OK: snapshot '%1' destroyed."), QStringLiteral("OK: snapshot '%1' destruído."));
        // v1.0.0: diálogo de atalhos
        c.insert(QStringLiteral("Refresh"), QStringLiteral("Atualizar"));
        c.insert(QStringLiteral("Create"), QStringLiteral("Criar"));
        c.insert(QStringLiteral("Activate temporary"), QStringLiteral("Ativar temporariamente"));
        c.insert(QStringLiteral("Mount"), QStringLiteral("Montar"));
        c.insert(QStringLiteral("Unmount"), QStringLiteral("Desmontar"));
        c.insert(QStringLiteral("Destroy selected"), QStringLiteral("Destruir selecionado(s)"));
        c.insert(QStringLiteral("Focus filter"), QStringLiteral("Ir para o filtro"));
        c.insert(QStringLiteral("Tabs: Information / Output / Help"), QStringLiteral("Abas: Informação / Saída / Ajuda"));
        c.insert(QStringLiteral("Snapshots tab"), QStringLiteral("Aba Snapshots"));
        c.insert(QStringLiteral("Next theme"), QStringLiteral("Próximo tema"));
        c.insert(QStringLiteral("Quit"), QStringLiteral("Sair"));
        // v1.1.0: Editar › Preferências
        c.insert(QStringLiteral("&Edit"), QStringLiteral("&Editar"));
        c.insert(QStringLiteral("&Preferences…"), QStringLiteral("&Preferências…"));
        c.insert(QStringLiteral("Preferences"), QStringLiteral("Preferências"));
        c.insert(QStringLiteral("Theme:"), QStringLiteral("Tema:"));
        c.insert(QStringLiteral("Language:"), QStringLiteral("Idioma:"));
        c.insert(QStringLiteral("System default"), QStringLiteral("Padrão do sistema"));
        c.insert(QStringLiteral("System (desktop Qt6 theme)"), QStringLiteral("Sistema (tema Qt6 do ambiente gráfico)"));
        g_cats.insert(QStringLiteral("pt_BR"), c);
    }
    {
        // en_US: só chaves que são identificadores, não texto em inglês.
        QHash<QString, QString> c;
        c.insert(QStringLiteral("Help body"), QStringLiteral("<h3>bectl — Boot Environment Manager</h3><p>Interface inspired by <b>OctoPKG</b> (list + detail tabs).</p><ul><li><b>Refresh</b> — bectl list -H</li><li><b>Create</b> — new BE, optionally cloned from another BE or from a snapshot (type <i>be@snapshot</i>)</li><li><b>Snapshots</b> tab — the selected BE's snapshots: create, clone into a new BE, destroy</li><li><b>Activate</b> — permanent or temporary (-t)</li><li><b>Mount / Unmount</b> — inspect a BE's files in a directory</li><li><b>Rename / Destroy</b> — with confirmation; the active BE is protected</li></ul><p>Privileged operations use <b>qt-sudo</b>. The window stays responsive while they run.</p><p>Shortcuts: F5 refresh · Ctrl+Q quit · Help › Keyboard shortcuts</p>"));
        g_cats.insert(QStringLiteral("en_US"), c);
    }
    {
        QHash<QString, QString> c;
        c.insert(QStringLiteral("&File"), QStringLiteral("&Archivo"));
        c.insert(QStringLiteral("&Actions"), QStringLiteral("&Acciones"));
        c.insert(QStringLiteral("&Theme"), QStringLiteral("&Tema"));
        c.insert(QStringLiteral("&Help"), QStringLiteral("A&yuda"));
        c.insert(QStringLiteral("&Quit"), QStringLiteral("&Salir"));
        c.insert(QStringLiteral("&Refresh"), QStringLiteral("Act&ualizar"));
        c.insert(QStringLiteral("&Create…"), QStringLiteral("&Crear…"));
        c.insert(QStringLiteral("A&ctivate"), QStringLiteral("&Activar"));
        c.insert(QStringLiteral("Activate (&temporary)"), QStringLiteral("Activar (&temporal)"));
        c.insert(QStringLiteral("&Rename…"), QStringLiteral("&Renombrar…"));
        c.insert(QStringLiteral("&Destroy selected"), QStringLiteral("&Destruir seleccionados"));
        c.insert(QStringLiteral("About bectl…"), QStringLiteral("Acerca de bectl…"));
        c.insert(QStringLiteral("Keyboard shortcuts"), QStringLiteral("Atajos de teclado"));
        c.insert(QStringLiteral("Shortcuts"), QStringLiteral("Atajos"));
        c.insert(QStringLiteral("Language"), QStringLiteral("Idioma"));
        c.insert(QStringLiteral("Ready."), QStringLiteral("Listo."));
        c.insert(QStringLiteral("0 BE(s)"), QStringLiteral("0 BE(s)"));
        c.insert(QStringLiteral("%1 BE(s)"), QStringLiteral("%1 BE(s)"));
        c.insert(QStringLiteral("Filter by name…"), QStringLiteral("Filtrar por nombre…"));
        c.insert(QStringLiteral("Boot Environment"), QStringLiteral("Boot Environment"));
        c.insert(QStringLiteral("Active"), QStringLiteral("Activo"));
        c.insert(QStringLiteral("Mountpoint"), QStringLiteral("Montaje"));
        c.insert(QStringLiteral("Space"), QStringLiteral("Espacio"));
        c.insert(QStringLiteral("Created"), QStringLiteral("Creado"));
        c.insert(QStringLiteral("Information"), QStringLiteral("Información"));
        c.insert(QStringLiteral("Output"), QStringLiteral("Salida"));
        c.insert(QStringLiteral("Help"), QStringLiteral("Ayuda"));
        c.insert(QStringLiteral("Select a Boot Environment to see details."), QStringLiteral("Seleccione un Boot Environment para ver detalles."));
        c.insert(QStringLiteral("Command output (bectl / qt-sudo)…"), QStringLiteral("Salida de operaciones (bectl / qt-sudo)…"));
        c.insert(QStringLiteral("Listing Boot Environments…"), QStringLiteral("Listando Boot Environments…"));
        c.insert(QStringLiteral("Error listing."), QStringLiteral("Error al listar."));
        c.insert(QStringLiteral("Error"), QStringLiteral("Error"));
        c.insert(QStringLiteral("List updated (%1 BE(s))."), QStringLiteral("Lista actualizada (%1 BE(s))."));
        c.insert(QStringLiteral("ERROR: %1"), QStringLiteral("ERROR: %1"));
        c.insert(QStringLiteral("Active now (N)"), QStringLiteral("Activo ahora (N)"));
        c.insert(QStringLiteral("Active on reboot (R)"), QStringLiteral("Activo en el próximo arranque (R)"));
        c.insert(QStringLiteral("Inactive"), QStringLiteral("Inactivo"));
        c.insert(QStringLiteral("Active now"), QStringLiteral("Activo ahora"));
        c.insert(QStringLiteral("Active on next boot"), QStringLiteral("Activo en el próximo arranque"));
        c.insert(QStringLiteral("State"), QStringLiteral("Estado"));
        c.insert(QStringLiteral("Create Boot Environment"), QStringLiteral("Crear Boot Environment"));
        c.insert(QStringLiteral("Name:"), QStringLiteral("Nombre:"));
        c.insert(QStringLiteral("Clone from:"), QStringLiteral("Clonar de:"));
        c.insert(QStringLiteral("(Current Boot Environment)"), QStringLiteral("(Boot Environment actual)"));
        c.insert(QStringLiteral("Create recursively (-r)"), QStringLiteral("Crear de forma recursiva (-r)"));
        c.insert(QStringLiteral("Name"), QStringLiteral("Nombre"));
        c.insert(QStringLiteral("Please enter a name."), QStringLiteral("Introduzca un nombre."));
        c.insert(QStringLiteral("Creating… (qt-sudo may ask for a password)"), QStringLiteral("Creando… (qt-sudo puede pedir contraseña)"));
        c.insert(QStringLiteral("Success"), QStringLiteral("Éxito"));
        c.insert(QStringLiteral("Boot Environment '%1' created."), QStringLiteral("Boot Environment '%1' creado."));
        c.insert(QStringLiteral("OK: '%1' created."), QStringLiteral("OK: '%1' creado."));
        c.insert(QStringLiteral("Selection"), QStringLiteral("Selección"));
        c.insert(QStringLiteral("Select exactly one Boot Environment."), QStringLiteral("Seleccione exactamente un Boot Environment."));
        c.insert(QStringLiteral("Activate"), QStringLiteral("Activar"));
        c.insert(QStringLiteral("Activate '%1' permanently?"), QStringLiteral("¿Activar permanentemente '%1'?"));
        c.insert(QStringLiteral("Activate (temporary)"), QStringLiteral("Activar (temporal)"));
        c.insert(QStringLiteral("Activate '%1' temporarily?"), QStringLiteral("¿Activar temporalmente '%1'?"));
        c.insert(QStringLiteral("OK: activated."), QStringLiteral("OK: activado."));
        c.insert(QStringLiteral("OK: activated (temporary)."), QStringLiteral("OK: activado (temporal)."));
        c.insert(QStringLiteral("Rename"), QStringLiteral("Renombrar"));
        c.insert(QStringLiteral("New name:"), QStringLiteral("Nuevo nombre:"));
        c.insert(QStringLiteral("OK: renamed."), QStringLiteral("OK: renombrado."));
        c.insert(QStringLiteral("Select one or more Boot Environments."), QStringLiteral("Seleccione uno o más Boot Environments."));
        c.insert(QStringLiteral("Active BE"), QStringLiteral("BE activo"));
        c.insert(QStringLiteral("Skipped (active):\n• %1"), QStringLiteral("Omitidos (activos):\n• %1"));
        c.insert(QStringLiteral("Confirm destroy"), QStringLiteral("Confirmar destrucción"));
        c.insert(QStringLiteral("WARNING: destroying '%1' is irreversible.\nContinue?"), QStringLiteral("ATENCIÓN: destruir '%1' es irreversible.\n¿Continuar?"));
        c.insert(QStringLiteral("WARNING: destroying %1 Boot Environments is irreversible:\n• %2\nContinue?"), QStringLiteral("ATENCIÓN: destruir %1 Boot Environments es irreversible:\n• %2\n¿Continuar?"));
        c.insert(QStringLiteral("Destroyed (%1):\n• %2\n"), QStringLiteral("Destruidos (%1):\n• %2\n"));
        c.insert(QStringLiteral("Failures (%1):\n• %2"), QStringLiteral("Fallos (%1):\n• %2"));
        c.insert(QStringLiteral("Result"), QStringLiteral("Resultado"));
        c.insert(QStringLiteral("Done."), QStringLiteral("Hecho."));
        c.insert(QStringLiteral("Theme: %1"), QStringLiteral("Tema: %1"));
        c.insert(QStringLiteral("About bectl"), QStringLiteral("Acerca de bectl"));
        c.insert(QStringLiteral("bectl — Boot Environment Manager"), QStringLiteral("bectl — Gestor de Boot Environments"));
        c.insert(QStringLiteral("Graphical manager for FreeBSD Boot Environments."), QStringLiteral("Gestor gráfico de Boot Environments (FreeBSD)."));
        c.insert(QStringLiteral("UI inspired by OctoPKG (main list + Information / Output / Help tabs)."), QStringLiteral("Interfaz inspirada en OctoPKG (lista principal + pestañas Información / Salida / Ayuda)."));
        c.insert(QStringLiteral("Logic: bectl · privilege: qt-sudo"), QStringLiteral("Lógica: bectl · privilegios: qt-sudo"));
        c.insert(QStringLiteral("Main"), QStringLiteral("Principal"));
        c.insert(QStringLiteral("English (US)"), QStringLiteral("Inglés (EE. UU.)"));
        c.insert(QStringLiteral("Portuguese (Brazil)"), QStringLiteral("Portugués (Brasil)"));
        c.insert(QStringLiteral("Spanish (Spain)"), QStringLiteral("Español (España)"));
        c.insert(QStringLiteral("Help body"), QStringLiteral("<h3>bectl — Gestor de Boot Environments</h3><p>Interfaz inspirada en <b>OctoPKG</b> (lista + pestañas).</p><ul><li><b>Actualizar</b> — bectl list -H</li><li><b>Crear</b> — nuevo BE, opcionalmente clonado de otro BE o de un snapshot (escriba <i>be@snapshot</i>)</li><li>Pestaña <b>Snapshots</b> — snapshots del BE seleccionado: crear, clonar en un nuevo BE, destruir</li><li><b>Activar</b> — permanente o temporal (-t)</li><li><b>Montar / Desmontar</b> — inspeccionar los archivos de un BE en un directorio</li><li><b>Renombrar / Destruir</b> — con confirmación; BE activo protegido</li></ul><p>Las operaciones privilegiadas usan <b>qt-sudo</b>. La ventana sigue respondiendo mientras se ejecutan.</p><p>Atajos: F5 actualizar · Ctrl+Q salir · Ayuda › Atajos de teclado</p>"));
        // Mensagens do BectlManager
        c.insert(QStringLiteral("'%1' not found. Install the security/qt-sudo package (pkg install qt-sudo)."), QStringLiteral("'%1' no encontrado. Instale el paquete security/qt-sudo (pkg install qt-sudo)."));
        c.insert(QStringLiteral("Failed to start '%1': %2"), QStringLiteral("No se pudo iniciar '%1': %2"));
        c.insert(QStringLiteral("Command timed out (%1 ms): %2"), QStringLiteral("El comando excedió el tiempo límite (%1 ms): %2"));
        c.insert(QStringLiteral("Process crashed: %1"), QStringLiteral("El proceso terminó abruptamente: %1"));
        c.insert(QStringLiteral("Command failed with exit code %1"), QStringLiteral("El comando falló con código %1"));
        c.insert(QStringLiteral("Snapshot name must contain '@' (e.g. default@before-upgrade)"), QStringLiteral("El nombre del snapshot debe contener '@' (ej: default@antes-actualizacion)"));
        c.insert(QStringLiteral("Invalid boot environment name: %1"), QStringLiteral("Nombre de Boot Environment no válido: %1"));
        c.insert(QStringLiteral("Invalid snapshot name: %1"), QStringLiteral("Nombre de snapshot no válido: %1"));
        c.insert(QStringLiteral("Mount point must be an absolute path: %1"), QStringLiteral("El punto de montaje debe ser una ruta absoluta: %1"));
        c.insert(QStringLiteral("%1: cannot destroy the currently active BE"), QStringLiteral("%1: no se puede destruir el BE activo en este momento"));
        // v0.2.0: operações assíncronas, montagem e snapshots
        c.insert(QStringLiteral("&Mount…"), QStringLiteral("&Montar…"));
        c.insert(QStringLiteral("&Unmount"), QStringLiteral("&Desmontar"));
        c.insert(QStringLiteral("Create &snapshot…"), QStringLiteral("Crear &snapshot…"));
        c.insert(QStringLiteral("Create snapshot"), QStringLiteral("Crear snapshot"));
        c.insert(QStringLiteral("Boot Environment:"), QStringLiteral("Boot Environment:"));
        c.insert(QStringLiteral("Snapshot name:"), QStringLiteral("Nombre del snapshot:"));
        c.insert(QStringLiteral("Mount Boot Environment"), QStringLiteral("Montar Boot Environment"));
        c.insert(QStringLiteral("Mount point:"), QStringLiteral("Punto de montaje:"));
        c.insert(QStringLiteral("Browse…"), QStringLiteral("Examinar…"));
        c.insert(QStringLiteral("Leave empty to let bectl create a temporary directory."), QStringLiteral("Déjelo vacío para que bectl cree un directorio temporal."));
        c.insert(QStringLiteral("Activating… (qt-sudo may ask for a password)"), QStringLiteral("Activando… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("Renaming… (qt-sudo may ask for a password)"), QStringLiteral("Renombrando… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("Destroying… (qt-sudo may ask for a password)"), QStringLiteral("Destruyendo… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("Mounting… (qt-sudo may ask for a password)"), QStringLiteral("Montando… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("Unmounting… (qt-sudo may ask for a password)"), QStringLiteral("Desmontando… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("Creating snapshot… (qt-sudo may ask for a password)"), QStringLiteral("Creando snapshot… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("OK: '%1' mounted."), QStringLiteral("OK: '%1' montado."));
        c.insert(QStringLiteral("OK: '%1' unmounted."), QStringLiteral("OK: '%1' desmontado."));
        c.insert(QStringLiteral("Wait for the current operation to finish."), QStringLiteral("Espere a que termine la operación en curso."));
        // v0.3.0: aba Snapshots
        c.insert(QStringLiteral("C&lone snapshot…"), QStringLiteral("C&lonar snapshot…"));
        c.insert(QStringLiteral("Destroy s&napshot"), QStringLiteral("Destruir s&napshot"));
        c.insert(QStringLiteral("Snapshots"), QStringLiteral("Snapshots"));
        c.insert(QStringLiteral("Snapshot"), QStringLiteral("Snapshot"));
        c.insert(QStringLiteral("Select a snapshot."), QStringLiteral("Seleccione un snapshot."));
        c.insert(QStringLiteral("Clone snapshot"), QStringLiteral("Clonar snapshot"));
        c.insert(QStringLiteral("New Boot Environment name:"), QStringLiteral("Nombre del nuevo Boot Environment:"));
        c.insert(QStringLiteral("Cloning… (qt-sudo may ask for a password)"), QStringLiteral("Clonando… (qt-sudo puede pedir la contraseña)"));
        c.insert(QStringLiteral("OK: '%1' created from '%2'."), QStringLiteral("OK: '%1' creado a partir de '%2'."));
        c.insert(QStringLiteral("OK: snapshot '%1' created."), QStringLiteral("OK: snapshot '%1' creado."));
        c.insert(QStringLiteral("OK: snapshot '%1' destroyed."), QStringLiteral("OK: snapshot '%1' destruido."));
        // v1.0.0: diálogo de atalhos
        c.insert(QStringLiteral("Refresh"), QStringLiteral("Actualizar"));
        c.insert(QStringLiteral("Create"), QStringLiteral("Crear"));
        c.insert(QStringLiteral("Activate temporary"), QStringLiteral("Activar temporalmente"));
        c.insert(QStringLiteral("Mount"), QStringLiteral("Montar"));
        c.insert(QStringLiteral("Unmount"), QStringLiteral("Desmontar"));
        c.insert(QStringLiteral("Destroy selected"), QStringLiteral("Destruir seleccionado(s)"));
        c.insert(QStringLiteral("Focus filter"), QStringLiteral("Ir al filtro"));
        c.insert(QStringLiteral("Tabs: Information / Output / Help"), QStringLiteral("Pestañas: Información / Salida / Ayuda"));
        c.insert(QStringLiteral("Snapshots tab"), QStringLiteral("Pestaña Snapshots"));
        c.insert(QStringLiteral("Next theme"), QStringLiteral("Siguiente tema"));
        c.insert(QStringLiteral("Quit"), QStringLiteral("Salir"));
        // v1.1.0: Editar › Preferências
        c.insert(QStringLiteral("&Edit"), QStringLiteral("&Editar"));
        c.insert(QStringLiteral("&Preferences…"), QStringLiteral("&Preferencias…"));
        c.insert(QStringLiteral("Preferences"), QStringLiteral("Preferencias"));
        c.insert(QStringLiteral("Theme:"), QStringLiteral("Tema:"));
        c.insert(QStringLiteral("Language:"), QStringLiteral("Idioma:"));
        c.insert(QStringLiteral("System default"), QStringLiteral("Predeterminado del sistema"));
        c.insert(QStringLiteral("System (desktop Qt6 theme)"), QStringLiteral("Sistema (tema Qt6 del entorno gráfico)"));
        g_cats.insert(QStringLiteral("es_ES"), c);
    }
}
} // anon

void setLocale(const QString &locale)
{
    ensureInit();
    if (locale == QLatin1String("pt_BR") || locale == QLatin1String("pt"))
        g_locale = QStringLiteral("pt_BR");
    else if (locale.startsWith(QLatin1String("es")))
        g_locale = QStringLiteral("es_ES");
    else
        g_locale = QStringLiteral("en_US");
}

void initFromSystem()
{
    ensureInit();
    const QString name = QLocale::system().name(); // e.g. en_US, pt_BR, es_ES
    if (name.startsWith(QLatin1String("pt")))
        setLocale(QStringLiteral("pt_BR"));
    else if (name.startsWith(QLatin1String("es")))
        setLocale(QStringLiteral("es_ES"));
    else
        setLocale(QStringLiteral("en_US"));
}

QString locale() { return g_locale; }

QStringList availableLocales()
{
    return {QStringLiteral("en_US"), QStringLiteral("pt_BR"), QStringLiteral("es_ES")};
}

QString localeDisplayName(const QString &loc)
{
    if (loc == QLatin1String("pt_BR")) return tr("Portuguese (Brazil)");
    if (loc == QLatin1String("es_ES")) return tr("Spanish (Spain)");
    return tr("English (US)");
}

QString tr(const char *key)
{
    return tr(QString::fromUtf8(key));
}

QString tr(const QString &key)
{
    ensureInit();
    const auto it = g_cats.find(g_locale);
    if (it == g_cats.end())
        return key;
    const auto jt = it->find(key);
    if (jt == it->end())
        return key;
    return *jt;
}

QString tr1(const char *key, const QString &a1)
{
    QString s = tr(key);
    s.replace(QStringLiteral("%1"), a1);
    return s;
}

QString tr2(const char *key, const QString &a1, const QString &a2)
{
    QString s = tr(key);
    s.replace(QStringLiteral("%1"), a1);
    s.replace(QStringLiteral("%2"), a2);
    return s;
}

} // namespace I18n