#include "mainwindow.h"
#include "themes.h"
#include "i18n.h"
#include <QKeyEvent>
#include <QShortcut>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QApplication>
#include <QStyle>
#include <QDateTime>
#include <QMenuBar>
#include <QKeySequence>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_mgr(true)
{
    setWindowTitle(QStringLiteral("bectl — Boot Environment Manager"));
    setMinimumSize(960, 600);
    resize(1040, 680);

    setupMenus();
    setupToolbar();
    setupCentral();
    setupStatus();
    setupShortcuts();
    applyTheme(m_themeId);
    retranslateUi();

    refreshList();
}

void MainWindow::applyTheme(const QString &id)
{
    m_themeId = id.isEmpty() ? QStringLiteral("octopkg") : id;
    qApp->setStyle(QStringLiteral("Fusion"));
    setStyleSheet(themeStyleSheet(m_themeId));
    if (m_statusLabel)
        m_statusLabel->setText(I18n::tr("Theme: %1").arg(themeLabel(m_themeId)));
    /* recolor table rows */
    try {
        fillTable(m_mgr.listEnvironments());
    } catch (...) {
    }
}

void MainWindow::onThemeTriggered(QAction *act)
{
    if (!act) return;
    applyTheme(act->data().toString());
}

void MainWindow::setupMenus()
{
    m_fileMenu = menuBar()->addMenu(QStringLiteral("&File"));
    m_actQuit = m_fileMenu->addAction(style()->standardIcon(QStyle::SP_DialogCloseButton),
                                      QStringLiteral("&Quit"), this, &QWidget::close);

    m_actMenu = menuBar()->addMenu(QStringLiteral("&Actions"));
    m_actRefresh = m_actMenu->addAction(style()->standardIcon(QStyle::SP_BrowserReload),
                                        QStringLiteral("&Refresh"), this, &MainWindow::refreshList);
    m_actMenu->addSeparator();
    m_actCreate = m_actMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder),
                                       QStringLiteral("&Create…"), this, &MainWindow::onCreate);
    m_actActivate = m_actMenu->addAction(style()->standardIcon(QStyle::SP_DialogApplyButton),
                                         QStringLiteral("A&ctivate"), this, &MainWindow::onActivate);
    m_actActivateTemp = m_actMenu->addAction(style()->standardIcon(QStyle::SP_DialogYesButton),
                                             QStringLiteral("Activate (&temporary)"), this, &MainWindow::onActivateTemp);
    m_actRename = m_actMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView),
                                       QStringLiteral("&Rename…"), this, &MainWindow::onRename);
    m_actMenu->addSeparator();
    m_actDestroy = m_actMenu->addAction(style()->standardIcon(QStyle::SP_TrashIcon),
                                        QStringLiteral("&Destroy selected"), this, &MainWindow::onDestroy);

    m_themeMenu = menuBar()->addMenu(QStringLiteral("&Theme"));
    m_themeGroup = new QActionGroup(this);
    m_themeGroup->setExclusive(true);
    for (const QString &tid : themeIds()) {
        auto *a = m_themeMenu->addAction(themeLabel(tid));
        a->setCheckable(true);
        a->setData(tid);
        m_themeGroup->addAction(a);
        if (tid == m_themeId)
            a->setChecked(true);
    }
    connect(m_themeGroup, &QActionGroup::triggered, this, &MainWindow::onThemeTriggered);

    m_langMenu = menuBar()->addMenu(QStringLiteral("Language"));
    m_langGroup = new QActionGroup(this);
    m_langGroup->setExclusive(true);
    for (const QString &loc : I18n::availableLocales()) {
        auto *a = m_langMenu->addAction(I18n::localeDisplayName(loc));
        a->setCheckable(true);
        a->setData(loc);
        m_langGroup->addAction(a);
        if (loc == I18n::locale())
            a->setChecked(true);
    }
    connect(m_langGroup, &QActionGroup::triggered, this, &MainWindow::onLanguageTriggered);

    m_helpMenu = menuBar()->addMenu(QStringLiteral("&Help"));
    m_actAbout = m_helpMenu->addAction(QStringLiteral("About bectl…"), this, &MainWindow::onAbout);
    m_actShortcuts = m_helpMenu->addAction(QStringLiteral("Keyboard shortcuts"), this, [this]() {
        QMessageBox::information(this, I18n::tr("Shortcuts"),
            QStringLiteral(
                "F5 / Ctrl+R     Refresh\n"
                "Ctrl+N          Create\n"
                "Ctrl+Return     Activate\n"
                "Ctrl+T          Activate temporary\n"
                "F2              Rename\n"
                "Delete          Destroy selected\n"
                "Ctrl+F          Focus filter\n"
                "Ctrl+1 / 2 / 3  Tabs\n"
                "Ctrl+Q          Quit\n"
                "Ctrl+Shift+T    Next theme"
            ));
    });
}

void MainWindow::onLanguageTriggered(QAction *act)
{
    if (!act) return;
    I18n::setLocale(act->data().toString());
    retranslateUi();
    try {
        fillTable(m_mgr.listEnvironments());
    } catch (...) {
    }
}

void MainWindow::retranslateUi()
{
    setWindowTitle(I18n::tr("bectl — Boot Environment Manager"));
    if (m_fileMenu) m_fileMenu->setTitle(I18n::tr("&File"));
    if (m_actMenu) m_actMenu->setTitle(I18n::tr("&Actions"));
    if (m_themeMenu) m_themeMenu->setTitle(I18n::tr("&Theme"));
    if (m_langMenu) m_langMenu->setTitle(I18n::tr("Language"));
    if (m_helpMenu) m_helpMenu->setTitle(I18n::tr("&Help"));

    if (m_actQuit) m_actQuit->setText(I18n::tr("&Quit"));
    if (m_actRefresh) m_actRefresh->setText(I18n::tr("&Refresh"));
    if (m_actCreate) m_actCreate->setText(I18n::tr("&Create…"));
    if (m_actActivate) m_actActivate->setText(I18n::tr("A&ctivate"));
    if (m_actActivateTemp) m_actActivateTemp->setText(I18n::tr("Activate (&temporary)"));
    if (m_actRename) m_actRename->setText(I18n::tr("&Rename…"));
    if (m_actDestroy) m_actDestroy->setText(I18n::tr("&Destroy selected"));
    if (m_actAbout) m_actAbout->setText(I18n::tr("About bectl…"));
    if (m_actShortcuts) m_actShortcuts->setText(I18n::tr("Keyboard shortcuts"));

    if (m_filter)
        m_filter->setPlaceholderText(I18n::tr("Filter by name…"));
    if (m_table) {
        m_table->setHorizontalHeaderLabels({
            QString(),
            I18n::tr("Boot Environment"),
            I18n::tr("Active"),
            I18n::tr("Mountpoint"),
            I18n::tr("Space"),
            I18n::tr("Created")
        });
    }
    if (m_tabs) {
        m_tabs->setTabText(0, I18n::tr("Information"));
        m_tabs->setTabText(1, I18n::tr("Output"));
        m_tabs->setTabText(2, I18n::tr("Help"));
    }
    if (m_info)
        m_info->setPlaceholderText(I18n::tr("Select a Boot Environment to see details."));
    if (m_output)
        m_output->setPlaceholderText(I18n::tr("Command output (bectl / qt-sudo)…"));
    if (m_tabs && m_tabs->widget(2)) {
        if (auto *help = qobject_cast<QTextEdit *>(m_tabs->widget(2)))
            help->setHtml(I18n::tr("Help body"));
    }
    if (m_statusLabel && m_statusLabel->text().isEmpty())
        m_statusLabel->setText(I18n::tr("Ready."));
    if (m_toolbar)
        m_toolbar->setWindowTitle(I18n::tr("Main"));

    /* refresh language menu labels */
    if (m_langGroup) {
        for (QAction *a : m_langGroup->actions())
            a->setText(I18n::localeDisplayName(a->data().toString()));
    }
}

void MainWindow::setupShortcuts()
{
    m_actRefresh->setShortcut(QKeySequence(QStringLiteral("F5")));
    m_actRefresh->setShortcuts({QKeySequence::Refresh, QKeySequence(QStringLiteral("Ctrl+R"))});
    m_actCreate->setShortcut(QKeySequence(QStringLiteral("Ctrl+N")));
    m_actActivate->setShortcut(QKeySequence(QStringLiteral("Ctrl+Return")));
    m_actActivateTemp->setShortcut(QKeySequence(QStringLiteral("Ctrl+T")));
    m_actRename->setShortcut(QKeySequence(QStringLiteral("F2")));
    m_actDestroy->setShortcut(QKeySequence(QStringLiteral("Delete")));
    m_actQuit->setShortcut(QKeySequence::Quit);

    m_actFocusFilter = new QAction(this);
    m_actFocusFilter->setShortcut(QKeySequence(QStringLiteral("Ctrl+F")));
    addAction(m_actFocusFilter);
    connect(m_actFocusFilter, &QAction::triggered, this, &MainWindow::focusFilter);

    auto *tab1 = new QShortcut(QKeySequence(QStringLiteral("Ctrl+1")), this);
    connect(tab1, &QShortcut::activated, this, &MainWindow::showTabInfo);
    auto *tab2 = new QShortcut(QKeySequence(QStringLiteral("Ctrl+2")), this);
    connect(tab2, &QShortcut::activated, this, &MainWindow::showTabOutput);
    auto *tab3 = new QShortcut(QKeySequence(QStringLiteral("Ctrl+3")), this);
    connect(tab3, &QShortcut::activated, this, &MainWindow::showTabHelp);

    auto *nextTheme = new QShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+T")), this);
    connect(nextTheme, &QShortcut::activated, this, [this]() {
        const QStringList ids = themeIds();
        int i = ids.indexOf(m_themeId);
        i = (i + 1) % ids.size();
        applyTheme(ids[i]);
        for (QAction *a : m_themeGroup->actions()) {
            if (a->data().toString() == m_themeId) {
                a->setChecked(true);
                break;
            }
        }
    });
}

void MainWindow::focusFilter()
{
    if (m_filter) {
        m_filter->setFocus(Qt::ShortcutFocusReason);
        m_filter->selectAll();
    }
}

void MainWindow::showTabInfo() { if (m_tabs) m_tabs->setCurrentIndex(0); }
void MainWindow::showTabOutput() { if (m_tabs) m_tabs->setCurrentIndex(1); }
void MainWindow::showTabHelp() { if (m_tabs) m_tabs->setCurrentIndex(2); }

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QMainWindow::keyPressEvent(event);
}


void MainWindow::setupToolbar()
{
    m_toolbar = addToolBar(I18n::tr("Main"));
    m_toolbar->setMovable(false);
    m_toolbar->setIconSize(QSize(22, 22));
    m_toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    m_toolbar->addAction(m_actRefresh);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_actCreate);
    m_toolbar->addAction(m_actActivate);
    m_toolbar->addAction(m_actActivateTemp);
    m_toolbar->addAction(m_actRename);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_actDestroy);

    auto *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);

    m_filter = new QLineEdit;
    m_filter->setPlaceholderText(I18n::tr("Filter by name…"));
    m_filter->setClearButtonEnabled(true);
    m_filter->setMaximumWidth(280);
    m_toolbar->addWidget(m_filter);
    connect(m_filter, &QLineEdit::textChanged, this, &MainWindow::onFilterChanged);
}

void MainWindow::setupCentral()
{
    auto *central = new QWidget;
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    m_table = new QTableWidget(0, 6);
    m_table->setHorizontalHeaderLabels({
        QString(), I18n::tr("Boot Environment"), I18n::tr("Active"),
        I18n::tr("Mountpoint"), I18n::tr("Space"), I18n::tr("Created")
    });
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->setShowGrid(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->setColumnWidth(2, 70);
    m_table->setColumnWidth(3, 140);
    m_table->setColumnWidth(4, 90);
    m_table->setIconSize(QSize(18, 18));
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::onSelectionChanged);

    m_tabs = new QTabWidget;
    m_tabs->setDocumentMode(true);

    m_info = new QTextEdit;
    m_info->setReadOnly(true);
    m_info->setPlaceholderText(I18n::tr("Select a Boot Environment to see details."));
    m_tabs->addTab(m_info, style()->standardIcon(QStyle::SP_FileDialogInfoView),
                   I18n::tr("Information"));

    m_output = new QTextEdit;
    m_output->setReadOnly(true);
    m_output->setPlaceholderText(I18n::tr("Command output (bectl / qt-sudo)…"));
    m_tabs->addTab(m_output, style()->standardIcon(QStyle::SP_ComputerIcon),
                   I18n::tr("Output"));

    auto *help = new QTextEdit;
    help->setReadOnly(true);
    help->setHtml(QStringLiteral(
        "<h3>bectl — Gerenciador de Boot Environments</h3>"
        "<p>Interface inspirada no <b>OctoPKG</b> (lista + abas de detalhe).</p>"
        "<ul>"
        "<li><b>Atualizar</b> — bectl list -H</li>"
        "<li><b>Criar</b> — novo BE (opcionalmente a partir de outro)</li>"
        "<li><b>Ativar</b> — permanente ou temporário (-t)</li>"
        "<li><b>Renomear / Destruir</b> — com confirmação; BE ativo protegido</li>"
        "</ul>"
        "<p>Operações privilegiadas usam <b>qt-sudo</b>.</p>"
        "<p>Atalhos: F5 atualizar · Ctrl+Q sair</p>"
    ));
    m_tabs->addTab(help, style()->standardIcon(QStyle::SP_DialogHelpButton),
                   I18n::tr("Help"));

    auto *split = new QSplitter(Qt::Vertical);
    split->addWidget(m_table);
    split->addWidget(m_tabs);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 1);
    split->setSizes({420, 180});

    layout->addWidget(split);
    setCentralWidget(central);
}

void MainWindow::setupStatus()
{
    m_statusLabel = new QLabel(I18n::tr("Ready."));
    m_countLabel = new QLabel(I18n::tr("0 BE(s)"));
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_countLabel);
}

void MainWindow::setBusy(bool busy)
{
    m_toolbar->setEnabled(!busy);
    m_table->setEnabled(!busy);
    m_filter->setEnabled(!busy);
    if (busy)
        QApplication::setOverrideCursor(Qt::WaitCursor);
    else
        QApplication::restoreOverrideCursor();
}

void MainWindow::appendOutput(const QString &line)
{
    const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    m_output->append(QStringLiteral("[%1] %2").arg(ts, line));
    m_tabs->setCurrentIndex(1);
}

void MainWindow::fillTable(const QVector<BootEnvironment> &list)
{
    const QString filter = m_filter ? m_filter->text().trimmed().toLower() : QString();
    m_table->setRowCount(0);

    int shown = 0;
    for (const auto &be : list) {
        if (!filter.isEmpty() && !be.name.toLower().contains(filter))
            continue;

        const int row = m_table->rowCount();
        m_table->insertRow(row);

        auto *iconItem = new QTableWidgetItem;
        if (be.isActiveNow()) {
            iconItem->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
            iconItem->setToolTip(I18n::tr("Active now (N)"));
        } else if (be.isActiveReboot()) {
            iconItem->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
            iconItem->setToolTip(I18n::tr("Active on reboot (R)"));
        } else {
            iconItem->setIcon(style()->standardIcon(QStyle::SP_FileIcon));
            iconItem->setToolTip(I18n::tr("Inactive"));
        }
        iconItem->setFlags(iconItem->flags() & ~Qt::ItemIsEditable);
        iconItem->setData(Qt::UserRole, be.name);
        m_table->setItem(row, 0, iconItem);

        auto *nameItem = new QTableWidgetItem(be.name);
        QString okFg, warnFg;
        themeRowColors(m_themeId, &okFg, &warnFg);
        if (be.isActiveNow()) {
            nameItem->setForeground(QColor(okFg));
            QFont f = nameItem->font();
            f.setBold(true);
            nameItem->setFont(f);
        } else if (be.isActiveReboot()) {
            nameItem->setForeground(QColor(warnFg));
        }
        m_table->setItem(row, 1, nameItem);
        m_table->setItem(row, 2, new QTableWidgetItem(be.active));
        m_table->setItem(row, 3, new QTableWidgetItem(be.mountpoint));
        m_table->setItem(row, 4, new QTableWidgetItem(be.space));
        m_table->setItem(row, 5, new QTableWidgetItem(be.created));
        ++shown;
    }

    m_countLabel->setText(I18n::tr("%1 BE(s)").arg(shown));
    if (m_table->rowCount() > 0)
        m_table->selectRow(0);
}

void MainWindow::refreshList()
{
    setBusy(true);
    m_statusLabel->setText(I18n::tr("Listing Boot Environments…"));
    QApplication::processEvents();
    try {
        auto list = m_mgr.listEnvironments();
        fillTable(list);
        m_statusLabel->setText(I18n::tr("Ready."));
        appendOutput(I18n::tr("List updated (%1 BE(s)).").arg(list.size()));
    } catch (const std::exception &e) {
        m_statusLabel->setText(I18n::tr("Error listing."));
        QMessageBox::critical(this, I18n::tr("Error"), QString::fromUtf8(e.what()));
        appendOutput(I18n::tr("ERROR: %1").arg(QString::fromUtf8(e.what())));
    }
    setBusy(false);
}

void MainWindow::onFilterChanged(const QString &)
{
    try {
        fillTable(m_mgr.listEnvironments());
    } catch (...) {
    }
}

void MainWindow::onSelectionChanged()
{
    const QString name = selectedSingle();
    if (name.isEmpty()) {
        m_info->clear();
        return;
    }
    try {
        auto opt = m_mgr.getEnvironment(name);
        if (opt)
            showBeInfo(*opt);
    } catch (...) {
        m_info->setPlainText(name);
    }
}

void MainWindow::showBeInfo(const BootEnvironment &be)
{
    QString okFg, warnFg;
    themeRowColors(m_themeId, &okFg, &warnFg);

    QString html;
    html += QStringLiteral("<h3>%1</h3>").arg(be.name.toHtmlEscaped());
    html += QStringLiteral("<table cellspacing='6'>");
    html += QStringLiteral("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(I18n::tr("Active"), be.active.toHtmlEscaped());
    html += QStringLiteral("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(I18n::tr("Mountpoint"), be.mountpoint.toHtmlEscaped());
    html += QStringLiteral("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(I18n::tr("Space"), be.space.toHtmlEscaped());
    html += QStringLiteral("<tr><td><b>%1</b></td><td>%2</td></tr>")
                .arg(I18n::tr("Created"), be.created.toHtmlEscaped());
    html += QStringLiteral("<tr><td><b>%1</b></td><td>").arg(I18n::tr("State"));
    if (be.isActiveNow())
        html += QStringLiteral("<span style='color:%1;font-weight:bold'>%2</span>")
                    .arg(okFg, I18n::tr("Active now"));
    else if (be.isActiveReboot())
        html += QStringLiteral("<span style='color:%1'>%2</span>")
                    .arg(warnFg, I18n::tr("Active on next boot"));
    else
        html += I18n::tr("Inactive");
    html += QStringLiteral("</td></tr></table>");
    m_info->setHtml(html);
}

QStringList MainWindow::selectedNames() const
{
    QStringList names;
    const auto ranges = m_table->selectionModel()->selectedRows();
    for (const auto &idx : ranges) {
        auto *it = m_table->item(idx.row(), 0);
        if (it)
            names << it->data(Qt::UserRole).toString();
    }
    return names;
}

QString MainWindow::selectedSingle() const
{
    const auto names = selectedNames();
    return names.size() == 1 ? names.first() : QString();
}

void MainWindow::onCreate()
{
    QDialog dlg(this);
    dlg.setWindowTitle(I18n::tr("Create Boot Environment"));
    auto *form = new QFormLayout(&dlg);
    auto *nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText(QStringLiteral("ex: antes-upgrade"));
    auto *srcCombo = new QComboBox;
    srcCombo->addItem(I18n::tr("(Current Boot Environment)"), QString());
    try {
        for (const auto &be : m_mgr.listEnvironments())
            srcCombo->addItem(be.name, be.name);
    } catch (...) {
    }
    auto *rec = new QCheckBox(I18n::tr("Create recursively (-r)"));
    form->addRow(I18n::tr("Name:"), nameEdit);
    form->addRow(I18n::tr("Clone from:"), srcCombo);
    form->addRow(QString(), rec);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QString name = nameEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, I18n::tr("Name"), I18n::tr("Please enter a name."));
        return;
    }
    const QString source = srcCombo->currentData().toString();
    setBusy(true);
    m_statusLabel->setText(I18n::tr("Creating… (qt-sudo may ask for a password)"));
    appendOutput(QStringLiteral("bectl create … %1").arg(name));
    QApplication::processEvents();
    try {
        if (source.isEmpty())
            m_mgr.create(name, QString(), rec->isChecked());
        else
            m_mgr.create(name, source, rec->isChecked());
        appendOutput(I18n::tr("OK: '%1' created.").arg(name));
        QMessageBox::information(this, I18n::tr("Success"),
                                 I18n::tr("Boot Environment '%1' created.").arg(name));
        refreshList();
    } catch (const std::exception &e) {
        appendOutput(I18n::tr("ERROR: %1").arg(QString::fromUtf8(e.what())));
        QMessageBox::critical(this, I18n::tr("Error"), QString::fromUtf8(e.what()));
    }
    setBusy(false);
}

void MainWindow::onActivate()
{
    const QString name = selectedSingle();
    if (name.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"),
                                 I18n::tr("Select exactly one Boot Environment."));
        return;
    }
    if (QMessageBox::question(this, I18n::tr("Activate"),
                              I18n::tr("Activate '%1' permanently?").arg(name))
        != QMessageBox::Yes)
        return;
    setBusy(true);
    appendOutput(QStringLiteral("bectl activate %1").arg(name));
    try {
        m_mgr.activate(name, false);
        appendOutput(I18n::tr("OK: activated."));
        refreshList();
    } catch (const std::exception &e) {
        appendOutput(I18n::tr("ERROR: %1").arg(QString::fromUtf8(e.what())));
        QMessageBox::critical(this, I18n::tr("Error"), QString::fromUtf8(e.what()));
    }
    setBusy(false);
}

void MainWindow::onActivateTemp()
{
    const QString name = selectedSingle();
    if (name.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"),
                                 I18n::tr("Select exactly one Boot Environment."));
        return;
    }
    if (QMessageBox::question(this, I18n::tr("Activate (temporary)"),
                              I18n::tr("Activate '%1' temporarily?").arg(name))
        != QMessageBox::Yes)
        return;
    setBusy(true);
    appendOutput(QStringLiteral("bectl activate -t %1").arg(name));
    try {
        m_mgr.activate(name, true);
        appendOutput(I18n::tr("OK: activated (temporary)."));
        refreshList();
    } catch (const std::exception &e) {
        appendOutput(I18n::tr("ERROR: %1").arg(QString::fromUtf8(e.what())));
        QMessageBox::critical(this, I18n::tr("Error"), QString::fromUtf8(e.what()));
    }
    setBusy(false);
}

void MainWindow::onRename()
{
    const QString name = selectedSingle();
    if (name.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"),
                                 I18n::tr("Select exactly one Boot Environment."));
        return;
    }
    bool ok = false;
    const QString neu = QInputDialog::getText(this, I18n::tr("Rename"),
                                              I18n::tr("New name:"), QLineEdit::Normal, name, &ok);
    if (!ok || neu.trimmed().isEmpty() || neu.trimmed() == name)
        return;
    setBusy(true);
    appendOutput(QStringLiteral("bectl rename %1 %2").arg(name, neu.trimmed()));
    try {
        m_mgr.rename(name, neu.trimmed());
        appendOutput(I18n::tr("OK: renamed."));
        refreshList();
    } catch (const std::exception &e) {
        appendOutput(I18n::tr("ERROR: %1").arg(QString::fromUtf8(e.what())));
        QMessageBox::critical(this, I18n::tr("Error"), QString::fromUtf8(e.what()));
    }
    setBusy(false);
}

void MainWindow::onDestroy()
{
    QStringList names = selectedNames();
    if (names.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"),
                                 I18n::tr("Select one or more Boot Environments."));
        return;
    }

    QStringList toDestroy;
    QStringList blocked;
    try {
        for (const QString &n : names) {
            auto opt = m_mgr.getEnvironment(n);
            if (opt && opt->isActiveNow())
                blocked << n;
            else
                toDestroy << n;
        }
    } catch (...) {
        toDestroy = names;
    }

    if (!blocked.isEmpty()) {
        QMessageBox::warning(this, I18n::tr("Active BE"),
                             I18n::tr("Skipped (active):\n• %1").arg(blocked.join(QStringLiteral("\n• "))));
    }
    if (toDestroy.isEmpty())
        return;

    const QString msg = toDestroy.size() == 1
        ? I18n::tr("WARNING: destroying '%1' is irreversible.\nContinue?").arg(toDestroy.first())
        : I18n::tr("WARNING: destroying %1 Boot Environments is irreversible:\n• %2\nContinue?")
              .arg(toDestroy.size())
              .arg(toDestroy.join(QStringLiteral("\n• ")));
    if (QMessageBox::warning(this, I18n::tr("Confirm destroy"), msg,
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        != QMessageBox::Yes)
        return;

    setBusy(true);
    appendOutput(QStringLiteral("bectl destroy -F %1").arg(toDestroy.join(QLatin1Char(' '))));
    try {
        auto r = m_mgr.destroyMany(toDestroy, true, false, true);
        QString report;
        if (!r.ok.isEmpty())
            report += I18n::tr("Destroyed (%1):\n• %2\n").arg(r.ok.size()).arg(r.ok.join(QStringLiteral("\n• ")));
        if (!r.errors.isEmpty())
            report += I18n::tr("Failures (%1):\n• %2").arg(r.errors.size()).arg(r.errors.join(QStringLiteral("\n• ")));
        appendOutput(report.trimmed());
        if (!r.errors.isEmpty())
            QMessageBox::warning(this, I18n::tr("Result"), report);
        else
            QMessageBox::information(this, I18n::tr("Result"),
                                     report.isEmpty() ? I18n::tr("Done.") : report);
        refreshList();
    } catch (const std::exception &e) {
        appendOutput(I18n::tr("ERROR: %1").arg(QString::fromUtf8(e.what())));
        QMessageBox::critical(this, I18n::tr("Error"), QString::fromUtf8(e.what()));
    }
    setBusy(false);
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, I18n::tr("About bectl"),
        QStringLiteral("<h3>%1</h3><p>%2</p><p>%3</p><p>%4</p><p>Qt6 · FreeBSD 15.1</p>")
            .arg(I18n::tr("bectl — Boot Environment Manager"),
                 I18n::tr("Graphical manager for FreeBSD Boot Environments."),
                 I18n::tr("UI inspired by OctoPKG (main list + Information / Output / Help tabs)."),
                 I18n::tr("Logic: bectl · privilege: qt-sudo")));
}
