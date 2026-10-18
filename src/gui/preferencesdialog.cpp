#include "preferencesdialog.h"
#include "i18n.h"
#include "themes.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>

PreferencesDialog::PreferencesDialog(const QString &theme, const QString &language, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(I18n::tr("Preferences"));
    auto *form = new QFormLayout(this);

    m_theme = new QComboBox;
    m_theme->setObjectName(QStringLiteral("themeCombo"));
    for (const QString &id : themeIds())
        m_theme->addItem(themeLabel(id), id);
    m_theme->setCurrentIndex(qMax(0, m_theme->findData(theme)));

    m_language = new QComboBox;
    m_language->setObjectName(QStringLiteral("languageCombo"));
    m_language->addItem(I18n::tr("System default"), QStringLiteral("system"));
    for (const QString &loc : I18n::availableLocales())
        m_language->addItem(I18n::localeDisplayName(loc), loc);
    m_language->setCurrentIndex(qMax(0, m_language->findData(language)));

    form->addRow(I18n::tr("Theme:"), m_theme);
    form->addRow(I18n::tr("Language:"), m_language);

    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString PreferencesDialog::theme() const
{
    return m_theme->currentData().toString();
}

QString PreferencesDialog::language() const
{
    return m_language->currentData().toString();
}
