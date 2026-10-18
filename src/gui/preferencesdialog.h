#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>

class QComboBox;

/**
 * Editar › Preferências: tema e idioma. "system" segue o ambiente gráfico
 * (estilo e paleta do Qt6) e o idioma do sistema, respectivamente.
 */
class PreferencesDialog : public QDialog
{
    Q_OBJECT
public:
    PreferencesDialog(const QString &theme, const QString &language, QWidget *parent = nullptr);

    QString theme() const;
    QString language() const;

private:
    QComboBox *m_theme = nullptr;
    QComboBox *m_language = nullptr;
};

#endif
