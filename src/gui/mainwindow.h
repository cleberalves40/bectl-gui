#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QTabWidget>
#include <QToolBar>
#include <QStatusBar>
#include <QSplitter>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QPalette>

#include <functional>

#include "bectlmanager.h"
#include "i18n.h"

class QThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    // bectlPath e useQtSudo vão para o BectlManager; o teste da interface os
    // usa para rodar com um bectl falso.
    explicit MainWindow(QWidget *parent = nullptr,
                        const QString &bectlPath = QString(),
                        bool useQtSudo = true);
    ~MainWindow() override;

    // Aplica e salva tema e idioma ("system" = os do ambiente gráfico).
    void applyPreferences(const QString &theme, const QString &language);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void refreshList();
    void onCreate();
    void onActivate();
    void onActivateTemp();
    void onRename();
    void onDestroy();
    void onMount();
    void onUnmount();
    void onSnapshot();
    void onCloneSnapshot();
    void onDestroySnapshot();
    void showTabSnapshots();
    void onFilterChanged(const QString &text);
    void onSelectionChanged();
    void onAbout();
    void onPreferences();
    void retranslateUi();
    void focusFilter();
    void showTabInfo();
    void showTabOutput();
    void showTabHelp();
    void appendOutput(const QString &line);

private:
    void setupUi();
    void setupMenus();
    void setupToolbar();
    void setupCentral();
    void setupStatus();
    void setupShortcuts();
    void applyTheme(const QString &id);
    void applyLanguage(const QString &language);
    void fillTable(const QVector<BootEnvironment> &list);
    void fillSnapshots();
    QString selectedSnapshot() const;
    QStringList selectedNames() const;
    QString selectedSingle() const;
    void setBusy(bool busy);
    void updateActions();
    void showBeInfo(const BootEnvironment &be);
    const BootEnvironment *findBe(const QString &name) const;
    void runAsync(const QString &status, const std::function<void()> &op,
                  const std::function<void()> &onSuccess);
    void reportError(const QString &message);

    BectlManager m_mgr;
    QVector<BootEnvironment> m_list;   // última saída de "bectl list"
    QVector<BeSnapshot> m_snapshots;   // última saída de "bectl list -s"
    QThread *m_worker = nullptr;       // operação em andamento, se houver
    QString m_themeId = QStringLiteral("system");
    QString m_language = QStringLiteral("system");
    QString m_systemStyle;     // estilo do ambiente gráfico, guardado ao iniciar
    QPalette m_systemPalette;  // idem, para restaurá-lo ao voltar a "system"

    QTableWidget *m_table = nullptr;
    QLineEdit *m_filter = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_countLabel = nullptr;
    QTextEdit *m_info = nullptr;
    QTextEdit *m_output = nullptr;
    QWidget *m_snapPage = nullptr;
    QTableWidget *m_snapTable = nullptr;
    QTabWidget *m_tabs = nullptr;
    QToolBar *m_toolbar = nullptr;
    QMenu *m_fileMenu = nullptr;
    QMenu *m_actMenu = nullptr;
    QMenu *m_editMenu = nullptr;
    QMenu *m_helpMenu = nullptr;
    QAction *m_actShortcuts = nullptr;
    QAction *m_actAbout = nullptr;

    QAction *m_actRefresh = nullptr;
    QAction *m_actCreate = nullptr;
    QAction *m_actActivate = nullptr;
    QAction *m_actActivateTemp = nullptr;
    QAction *m_actRename = nullptr;
    QAction *m_actDestroy = nullptr;
    QAction *m_actMount = nullptr;
    QAction *m_actUnmount = nullptr;
    QAction *m_actSnapshot = nullptr;
    QAction *m_actCloneSnapshot = nullptr;
    QAction *m_actDestroySnapshot = nullptr;
    QAction *m_actQuit = nullptr;
    QAction *m_actFocusFilter = nullptr;
    QAction *m_actPreferences = nullptr;
};

#endif
