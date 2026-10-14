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

#include "bectlmanager.h"
#include "i18n.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void refreshList();
    void onCreate();
    void onActivate();
    void onActivateTemp();
    void onRename();
    void onDestroy();
    void onFilterChanged(const QString &text);
    void onSelectionChanged();
    void onAbout();
    void onThemeTriggered(QAction *act);
    void onLanguageTriggered(QAction *act);
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
    void fillTable(const QVector<BootEnvironment> &list);
    QStringList selectedNames() const;
    QString selectedSingle() const;
    void setBusy(bool busy);
    void showBeInfo(const BootEnvironment &be);

    BectlManager m_mgr;
    QString m_themeId = QStringLiteral("octopkg");

    QTableWidget *m_table = nullptr;
    QLineEdit *m_filter = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_countLabel = nullptr;
    QTextEdit *m_info = nullptr;
    QTextEdit *m_output = nullptr;
    QTabWidget *m_tabs = nullptr;
    QToolBar *m_toolbar = nullptr;
    QActionGroup *m_themeGroup = nullptr;
    QActionGroup *m_langGroup = nullptr;
    QMenu *m_fileMenu = nullptr;
    QMenu *m_actMenu = nullptr;
    QMenu *m_themeMenu = nullptr;
    QMenu *m_langMenu = nullptr;
    QMenu *m_helpMenu = nullptr;
    QAction *m_actShortcuts = nullptr;
    QAction *m_actAbout = nullptr;

    QAction *m_actRefresh = nullptr;
    QAction *m_actCreate = nullptr;
    QAction *m_actActivate = nullptr;
    QAction *m_actActivateTemp = nullptr;
    QAction *m_actRename = nullptr;
    QAction *m_actDestroy = nullptr;
    QAction *m_actQuit = nullptr;
    QAction *m_actFocusFilter = nullptr;
};

#endif
