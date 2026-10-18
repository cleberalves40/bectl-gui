#include "mainwindow.h"
#include "themes.h"
#include "i18n.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QPushButton>
#include <QShortcut>
#include <QThread>

#include <memory>

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

MainWindow::MainWindow(QWidget *parent, const QString &bectlPath, bool useQtSudo)
    : QMainWindow(parent)
    , m_mgr(useQtSudo, QStringLiteral("qt-sudo"), bectlPath)
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

MainWindow::~MainWindow()
{
    // A operação em andamento usa m_mgr: espera antes de destruí-lo.
    if (m_worker)
        m_worker->wait();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_worker) {
        m_statusLabel->setText(I18n::tr("Wait for the current operation to finish."));
        event->ignore();
        return;
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::applyTheme(const QString &id)
{
    m_themeId = id.isEmpty() ? QStringLiteral("octopkg") : id;
    qApp->setStyle(QStringLiteral("Fusion"));
    setStyleSheet(themeStyleSheet(m_themeId));
    if (m_statusLabel)
        m_statusLabel->setText(I18n::tr("Theme: %1").arg(themeLabel(m_themeId)));
    /* recolor table rows */
    fillTable(m_list);
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
    m_actSnapshot = m_actMenu->addAction(style()->standardIcon(QStyle::SP_DialogSaveButton),
                                         QStringLiteral("Create &snapshot…"), this, &MainWindow::onSnapshot);
    m_actMenu->addSeparator();
    m_actMount = m_actMenu->addAction(style()->standardIcon(QStyle::SP_DriveHDIcon),
                                      QStringLiteral("&Mount…"), this, &MainWindow::onMount);
    m_actUnmount = m_actMenu->addAction(style()->standardIcon(QStyle::SP_MediaStop),
                                        QStringLiteral("&Unmount"), this, &MainWindow::onUnmount);
    m_actMenu->addSeparator();
    m_actCloneSnapshot = m_actMenu->addAction(style()->standardIcon(QStyle::SP_FileDialogNewFolder),
                                              QStringLiteral("C&lone snapshot…"), this, &MainWindow::onCloneSnapshot);
    m_actDestroySnapshot = m_actMenu->addAction(style()->standardIcon(QStyle::SP_TrashIcon),
                                                QStringLiteral("Destroy s&napshot"), this, &MainWindow::onDestroySnapshot);
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
                "Ctrl+Shift+S    Create snapshot\n"
                "Ctrl+M          Mount\n"
                "Ctrl+U          Unmount\n"
                "Delete          Destroy selected\n"
                "Ctrl+F          Focus filter\n"
                "Ctrl+1 / 2 / 3  Tabs\n"
                "Ctrl+4          Snapshots tab\n"
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
    fillTable(m_list);
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
    if (m_actSnapshot) m_actSnapshot->setText(I18n::tr("Create &snapshot…"));
    if (m_actMount) m_actMount->setText(I18n::tr("&Mount…"));
    if (m_actUnmount) m_actUnmount->setText(I18n::tr("&Unmount"));
    if (m_actCloneSnapshot) m_actCloneSnapshot->setText(I18n::tr("C&lone snapshot…"));
    if (m_actDestroySnapshot) m_actDestroySnapshot->setText(I18n::tr("Destroy s&napshot"));
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
        m_tabs->setTabText(m_tabs->indexOf(m_snapPage), I18n::tr("Snapshots"));
    }
    if (m_snapTable)
        m_snapTable->setHorizontalHeaderLabels({I18n::tr("Snapshot"), I18n::tr("Space"),
                                                I18n::tr("Created")});
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
    m_actSnapshot->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+S")));
    m_actMount->setShortcut(QKeySequence(QStringLiteral("Ctrl+M")));
    m_actUnmount->setShortcut(QKeySequence(QStringLiteral("Ctrl+U")));

    // Nomes usados pelo teste da interface (tests/tst_mainwindow.cpp).
    m_actRefresh->setObjectName(QStringLiteral("actRefresh"));
    m_actCreate->setObjectName(QStringLiteral("actCreate"));
    m_actActivate->setObjectName(QStringLiteral("actActivate"));
    m_actActivateTemp->setObjectName(QStringLiteral("actActivateTemp"));
    m_actRename->setObjectName(QStringLiteral("actRename"));
    m_actDestroy->setObjectName(QStringLiteral("actDestroy"));
    m_actSnapshot->setObjectName(QStringLiteral("actSnapshot"));
    m_actMount->setObjectName(QStringLiteral("actMount"));
    m_actUnmount->setObjectName(QStringLiteral("actUnmount"));
    m_actCloneSnapshot->setObjectName(QStringLiteral("actCloneSnapshot"));
    m_actDestroySnapshot->setObjectName(QStringLiteral("actDestroySnapshot"));
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
    auto *tab4 = new QShortcut(QKeySequence(QStringLiteral("Ctrl+4")), this);
    connect(tab4, &QShortcut::activated, this, &MainWindow::showTabSnapshots);

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
void MainWindow::showTabSnapshots() { if (m_tabs) m_tabs->setCurrentWidget(m_snapPage); }

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
    m_toolbar->addAction(m_actSnapshot);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_actMount);
    m_toolbar->addAction(m_actUnmount);
    m_toolbar->addSeparator();
    m_toolbar->addAction(m_actDestroy);

    auto *spacer = new QWidget;
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolbar->addWidget(spacer);

    m_filter = new QLineEdit;
    m_filter->setObjectName(QStringLiteral("filter"));
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
    m_table->setObjectName(QStringLiteral("table"));
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
    help->setHtml(I18n::tr("Help body"));
    m_tabs->addTab(help, style()->standardIcon(QStyle::SP_DialogHelpButton),
                   I18n::tr("Help"));

    // Snapshots do BE selecionado (de todos, se não houver um único selecionado).
    m_snapPage = new QWidget;
    auto *snapLayout = new QVBoxLayout(m_snapPage);
    snapLayout->setContentsMargins(0, 0, 0, 0);
    auto *snapBar = new QToolBar;
    snapBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    snapBar->setIconSize(QSize(16, 16));
    snapBar->addAction(m_actSnapshot);
    snapBar->addAction(m_actCloneSnapshot);
    snapBar->addAction(m_actDestroySnapshot);
    m_snapTable = new QTableWidget(0, 3);
    m_snapTable->setObjectName(QStringLiteral("snapTable"));
    m_snapTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_snapTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_snapTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_snapTable->setAlternatingRowColors(true);
    m_snapTable->verticalHeader()->setVisible(false);
    m_snapTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_snapTable->setHorizontalHeaderLabels({I18n::tr("Snapshot"), I18n::tr("Space"),
                                            I18n::tr("Created")});
    connect(m_snapTable, &QTableWidget::itemSelectionChanged, this, &MainWindow::updateActions);
    snapLayout->addWidget(snapBar);
    snapLayout->addWidget(m_snapTable);
    m_tabs->addTab(m_snapPage, style()->standardIcon(QStyle::SP_DialogSaveButton),
                   I18n::tr("Snapshots"));

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
    m_statusLabel->setObjectName(QStringLiteral("status"));
    m_countLabel = new QLabel(I18n::tr("0 BE(s)"));
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_countLabel);
}

void MainWindow::setBusy(bool busy)
{
    // Toda ação que chama o bectl fica desativada, inclusive pelos atalhos, e
    // também a troca de idioma, que altera o catálogo lido pela operação.
    for (QAction *a : {m_actRefresh, m_actCreate, m_actActivate, m_actActivateTemp, m_actRename,
                       m_actSnapshot, m_actMount, m_actUnmount, m_actDestroy,
                       m_actCloneSnapshot, m_actDestroySnapshot})
        a->setEnabled(!busy);
    m_langMenu->setEnabled(!busy);
    if (busy) {
        QApplication::setOverrideCursor(Qt::BusyCursor);
    } else {
        QApplication::restoreOverrideCursor();
        updateActions();
    }
}

// Habilita cada ação conforme a seleção atual.
void MainWindow::updateActions()
{
    if (m_worker)
        return;
    const QStringList names = selectedNames();
    const BootEnvironment *be = names.size() == 1 ? findBe(names.first()) : nullptr;
    m_actActivate->setEnabled(be);
    m_actActivateTemp->setEnabled(be);
    m_actRename->setEnabled(be);
    m_actMount->setEnabled(be && !be->isMounted());
    // O BE em uso está montado em "/" e não pode ser desmontado.
    m_actUnmount->setEnabled(be && be->isMounted() && !be->isActiveNow());
    m_actDestroy->setEnabled(!names.isEmpty());
    const bool snap = !selectedSnapshot().isEmpty();
    m_actCloneSnapshot->setEnabled(snap);
    m_actDestroySnapshot->setEnabled(snap);
}

// Executa op numa thread à parte, para a interface continuar respondendo
// enquanto o bectl roda e o qt-sudo espera a senha. Se op lançar exceção, o
// erro é exibido; senão, onSuccess roda na thread da interface.
void MainWindow::runAsync(const QString &status, const std::function<void()> &op,
                          const std::function<void()> &onSuccess)
{
    if (m_worker)
        return;
    auto error = std::make_shared<QString>();
    m_worker = QThread::create([op, error]() {
        try {
            op();
        } catch (const std::exception &e) {
            *error = QString::fromUtf8(e.what());
        }
    });
    connect(m_worker, &QThread::finished, this, [this, error, onSuccess]() {
        m_worker->deleteLater();
        m_worker = nullptr;
        setBusy(false);
        if (!error->isEmpty()) {
            reportError(*error);
            return;
        }
        m_statusLabel->setText(I18n::tr("Ready."));
        if (onSuccess)
            onSuccess();
    });
    setBusy(true);
    m_statusLabel->setText(status);
    m_worker->start();
}

void MainWindow::reportError(const QString &message)
{
    m_statusLabel->setText(I18n::tr("Error"));
    appendOutput(I18n::tr("ERROR: %1").arg(message));
    QMessageBox::critical(this, I18n::tr("Error"), message);
}
void MainWindow::appendOutput(const QString &line)
{
    const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("hh:mm:ss"));
    m_output->append(QStringLiteral("[%1] %2").arg(ts, line));
    m_tabs->setCurrentWidget(m_output);
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
    auto list = std::make_shared<QVector<BootEnvironment>>();
    auto snaps = std::make_shared<QVector<BeSnapshot>>();
    runAsync(I18n::tr("Listing Boot Environments…"),
             [this, list, snaps]() {
                 *list = m_mgr.listEnvironments();
                 *snaps = m_mgr.listSnapshots();
             },
             [this, list, snaps]() {
                 m_list = *list;
                 m_snapshots = *snaps;
                 fillTable(m_list);
                 fillSnapshots();
                 appendOutput(I18n::tr("List updated (%1 BE(s)).").arg(m_list.size()));
             });
}
void MainWindow::onFilterChanged(const QString &)
{
    fillTable(m_list);
}
void MainWindow::onSelectionChanged()
{
    fillSnapshots();
    updateActions();
    const QString name = selectedSingle();
    const BootEnvironment *be = name.isEmpty() ? nullptr : findBe(name);
    if (be)
        showBeInfo(*be);
    else
        m_info->clear();
}

void MainWindow::fillSnapshots()
{
    const QString be = selectedSingle();
    m_snapTable->setRowCount(0);
    for (const BeSnapshot &s : m_snapshots) {
        if (!be.isEmpty() && s.environment() != be)
            continue;
        const int row = m_snapTable->rowCount();
        m_snapTable->insertRow(row);
        m_snapTable->setItem(row, 0, new QTableWidgetItem(s.name));
        auto *space = new QTableWidgetItem(s.space);
        space->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        m_snapTable->setItem(row, 1, space);
        m_snapTable->setItem(row, 2, new QTableWidgetItem(s.created));
    }
    m_tabs->setTabText(m_tabs->indexOf(m_snapPage),
                       QStringLiteral("%1 (%2)").arg(I18n::tr("Snapshots")).arg(m_snapTable->rowCount()));
}

QString MainWindow::selectedSnapshot() const
{
    const auto rows = m_snapTable->selectionModel()->selectedRows();
    if (rows.size() != 1)
        return QString();
    const auto *it = m_snapTable->item(rows.first().row(), 0);
    return it ? it->text() : QString();
}

const BootEnvironment *MainWindow::findBe(const QString &name) const
{
    for (const auto &be : m_list) {
        if (be.name == name)
            return &be;
    }
    return nullptr;
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
    // Editável: aceita também um snapshot digitado, como "default@antes-upgrade".
    auto *srcCombo = new QComboBox;
    srcCombo->setEditable(true);
    srcCombo->addItem(I18n::tr("(Current Boot Environment)"), QString());
    for (const auto &be : m_list)
        srcCombo->addItem(be.name, be.name);
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
    const QString text = srcCombo->currentText().trimmed();
    const QString source = text == srcCombo->itemText(0) ? QString() : text;
    const bool recursive = rec->isChecked();
    appendOutput(QStringLiteral("bectl create … %1").arg(name));
    runAsync(I18n::tr("Creating… (qt-sudo may ask for a password)"),
             [this, name, source, recursive]() { m_mgr.create(name, source, recursive); },
             [this, name]() {
                 appendOutput(I18n::tr("OK: '%1' created.").arg(name));
                 QMessageBox::information(this, I18n::tr("Success"),
                                          I18n::tr("Boot Environment '%1' created.").arg(name));
                 refreshList();
             });
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
    appendOutput(QStringLiteral("bectl activate %1").arg(name));
    runAsync(I18n::tr("Activating… (qt-sudo may ask for a password)"),
             [this, name]() { m_mgr.activate(name, false); },
             [this]() {
                 appendOutput(I18n::tr("OK: activated."));
                 refreshList();
             });
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
    appendOutput(QStringLiteral("bectl activate -t %1").arg(name));
    runAsync(I18n::tr("Activating… (qt-sudo may ask for a password)"),
             [this, name]() { m_mgr.activate(name, true); },
             [this]() {
                 appendOutput(I18n::tr("OK: activated (temporary)."));
                 refreshList();
             });
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
                                              I18n::tr("New name:"), QLineEdit::Normal, name, &ok)
                            .trimmed();
    if (!ok || neu.isEmpty() || neu == name)
        return;
    appendOutput(QStringLiteral("bectl rename %1 %2").arg(name, neu));
    runAsync(I18n::tr("Renaming… (qt-sudo may ask for a password)"),
             [this, name, neu]() { m_mgr.rename(name, neu); },
             [this]() {
                 appendOutput(I18n::tr("OK: renamed."));
                 refreshList();
             });
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
    for (const QString &n : names) {
        const BootEnvironment *be = findBe(n);
        if (be && be->isActiveNow())
            blocked << n;
        else
            toDestroy << n;
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

    appendOutput(QStringLiteral("bectl destroy -F %1").arg(toDestroy.join(QLatin1Char(' '))));
    auto r = std::make_shared<DestroyManyResult>();
    runAsync(I18n::tr("Destroying… (qt-sudo may ask for a password)"),
             [this, toDestroy, r]() { *r = m_mgr.destroyMany(toDestroy, true, false, true); },
             [this, r]() {
                 QString report;
                 if (!r->ok.isEmpty())
                     report += I18n::tr("Destroyed (%1):\n• %2\n").arg(r->ok.size()).arg(r->ok.join(QStringLiteral("\n• ")));
                 if (!r->errors.isEmpty())
                     report += I18n::tr("Failures (%1):\n• %2").arg(r->errors.size()).arg(r->errors.join(QStringLiteral("\n• ")));
                 appendOutput(report.trimmed());
                 if (!r->errors.isEmpty())
                     QMessageBox::warning(this, I18n::tr("Result"), report);
                 else
                     QMessageBox::information(this, I18n::tr("Result"),
                                              report.isEmpty() ? I18n::tr("Done.") : report);
                 refreshList();
             });
}

void MainWindow::onMount()
{
    const QString name = selectedSingle();
    if (name.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"),
                                 I18n::tr("Select exactly one Boot Environment."));
        return;
    }
    QDialog dlg(this);
    dlg.setWindowTitle(I18n::tr("Mount Boot Environment"));
    auto *form = new QFormLayout(&dlg);
    auto *dirEdit = new QLineEdit;
    auto *browse = new QPushButton(I18n::tr("Browse…"));
    connect(browse, &QPushButton::clicked, &dlg, [&dlg, dirEdit]() {
        const QString dir = QFileDialog::getExistingDirectory(&dlg, I18n::tr("Mount point:"));
        if (!dir.isEmpty())
            dirEdit->setText(dir);
    });
    auto *row = new QHBoxLayout;
    row->addWidget(dirEdit);
    row->addWidget(browse);
    form->addRow(I18n::tr("Mount point:"), row);
    form->addRow(QString(), new QLabel(I18n::tr("Leave empty to let bectl create a temporary directory.")));
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QString dir = dirEdit->text().trimmed();
    appendOutput(QStringLiteral("bectl mount %1 %2").arg(name, dir).trimmed());
    auto out = std::make_shared<QString>();
    runAsync(I18n::tr("Mounting… (qt-sudo may ask for a password)"),
             [this, name, dir, out]() { *out = m_mgr.mount(name, dir); },
             [this, name, out]() {
                 if (!out->isEmpty())
                     appendOutput(*out);
                 appendOutput(I18n::tr("OK: '%1' mounted.").arg(name));
                 refreshList();
             });
}

void MainWindow::onUnmount()
{
    const QString name = selectedSingle();
    if (name.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"),
                                 I18n::tr("Select exactly one Boot Environment."));
        return;
    }
    appendOutput(QStringLiteral("bectl umount %1").arg(name));
    runAsync(I18n::tr("Unmounting… (qt-sudo may ask for a password)"),
             [this, name]() { m_mgr.unmount(name); },
             [this, name]() {
                 appendOutput(I18n::tr("OK: '%1' unmounted.").arg(name));
                 refreshList();
             });
}

void MainWindow::onSnapshot()
{
    QDialog dlg(this);
    dlg.setWindowTitle(I18n::tr("Create snapshot"));
    auto *form = new QFormLayout(&dlg);
    auto *beCombo = new QComboBox;
    for (const auto &be : m_list)
        beCombo->addItem(be.name);
    beCombo->setCurrentText(selectedSingle());
    auto *snapEdit = new QLineEdit;
    snapEdit->setPlaceholderText(QStringLiteral("ex: antes-upgrade"));
    auto *rec = new QCheckBox(I18n::tr("Create recursively (-r)"));
    form->addRow(I18n::tr("Boot Environment:"), beCombo);
    form->addRow(I18n::tr("Snapshot name:"), snapEdit);
    form->addRow(QString(), rec);
    auto *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    form->addRow(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    if (dlg.exec() != QDialog::Accepted)
        return;

    const QString snap = snapEdit->text().trimmed();
    if (beCombo->currentText().isEmpty() || snap.isEmpty()) {
        QMessageBox::warning(this, I18n::tr("Name"), I18n::tr("Please enter a name."));
        return;
    }
    const QString full = beCombo->currentText() + QLatin1Char('@') + snap;
    const bool recursive = rec->isChecked();
    appendOutput(QStringLiteral("bectl create %1%2").arg(recursive ? QStringLiteral("-r ") : QString(), full));
    runAsync(I18n::tr("Creating snapshot… (qt-sudo may ask for a password)"),
             [this, full, recursive]() { m_mgr.createSnapshot(full, recursive); },
             [this, full]() {
                 appendOutput(I18n::tr("OK: snapshot '%1' created.").arg(full));
                 refreshList();
             });
}

void MainWindow::onCloneSnapshot()
{
    const QString snap = selectedSnapshot();
    if (snap.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"), I18n::tr("Select a snapshot."));
        return;
    }
    bool ok = false;
    const QString name = QInputDialog::getText(this, I18n::tr("Clone snapshot"),
                                               I18n::tr("New Boot Environment name:"),
                                               QLineEdit::Normal, QString(), &ok)
                             .trimmed();
    if (!ok || name.isEmpty())
        return;
    appendOutput(QStringLiteral("bectl create -e %1 %2").arg(snap, name));
    runAsync(I18n::tr("Cloning… (qt-sudo may ask for a password)"),
             [this, name, snap]() { m_mgr.create(name, snap); },
             [this, name, snap]() {
                 appendOutput(I18n::tr("OK: '%1' created from '%2'.").arg(name, snap));
                 refreshList();
             });
}

void MainWindow::onDestroySnapshot()
{
    const QString snap = selectedSnapshot();
    if (snap.isEmpty()) {
        QMessageBox::information(this, I18n::tr("Selection"), I18n::tr("Select a snapshot."));
        return;
    }
    if (QMessageBox::warning(this, I18n::tr("Confirm destroy"),
                             I18n::tr("WARNING: destroying '%1' is irreversible.\nContinue?").arg(snap),
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
        != QMessageBox::Yes)
        return;
    appendOutput(QStringLiteral("bectl destroy %1").arg(snap));
    runAsync(I18n::tr("Destroying… (qt-sudo may ask for a password)"),
             [this, snap]() { m_mgr.destroy(snap); },
             [this, snap]() {
                 appendOutput(I18n::tr("OK: snapshot '%1' destroyed.").arg(snap));
                 refreshList();
             });
}
void MainWindow::onAbout()
{
    QMessageBox::about(this, I18n::tr("About bectl"),
        QStringLiteral("<h3>%1 %5</h3><p>%2</p><p>%3</p><p>%4</p><p>Qt6 · FreeBSD 14 / 15</p>")
            .arg(I18n::tr("bectl — Boot Environment Manager"),
                 I18n::tr("Graphical manager for FreeBSD Boot Environments."),
                 I18n::tr("UI inspired by OctoPKG (main list + Information / Output / Help tabs)."),
                 I18n::tr("Logic: bectl · privilege: qt-sudo"),
                 QApplication::applicationVersion()));
}
