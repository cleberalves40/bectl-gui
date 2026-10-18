#ifndef BECTL_THEMES_H
#define BECTL_THEMES_H

#include <QString>
#include <QStringList>

/** Temas portados da versão GTK3 (Catppuccin, Solarized, Selenized, Dracula, etc.) */
struct ThemeDef {
    const char *id;
    const char *label;
};

/** "system" (primeiro da lista) usa o estilo e a paleta do ambiente gráfico. */
QStringList themeIds();
QString themeLabel(const QString &id);
/** Folha de estilo Qt (QSS) equivalente às paletas GTK3 */
QString themeStyleSheet(const QString &id);
/** Cores de destaque para linhas da tabela (ativo / reboot). Para "system",
 *  darkBackground escolhe entre as cores para fundo claro ou escuro. */
void themeRowColors(const QString &id, QString *activeFg, QString *rebootFg,
                    bool darkBackground = false);

#endif
