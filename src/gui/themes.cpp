#include "themes.h"
#include "i18n.h"
#include <QMap>

static const char *kThemeIds[] = {
    "system",
    "octopkg",
    "frappe",
    "latte",
    "mocha",
    "macchiato",
    "solarized_dark",
    "solarized_light",
    "selenized_dark",
    "selenized_light",
    "dracula",
    "tokyo_night",
    "nord",
    "gruvbox",
    "one_dark",
    nullptr
};

QStringList themeIds()
{
    QStringList ids;
    for (int i = 0; kThemeIds[i]; ++i) ids << QLatin1String(kThemeIds[i]);
    return ids;
}

QString themeLabel(const QString &id)
{
    if (id == QLatin1String("system")) return I18n::tr("System (desktop Qt6 theme)");
    if (id == QLatin1String("octopkg")) return QStringLiteral("OctoPKG (claro)");
    if (id == QLatin1String("frappe")) return QStringLiteral("Catppuccin Frappé");
    if (id == QLatin1String("latte")) return QStringLiteral("Catppuccin Latte");
    if (id == QLatin1String("mocha")) return QStringLiteral("Catppuccin Mocha");
    if (id == QLatin1String("macchiato")) return QStringLiteral("Catppuccin Macchiato");
    if (id == QLatin1String("solarized_dark")) return QStringLiteral("Solarized Dark");
    if (id == QLatin1String("solarized_light")) return QStringLiteral("Solarized Light");
    if (id == QLatin1String("selenized_dark")) return QStringLiteral("Selenized Dark");
    if (id == QLatin1String("selenized_light")) return QStringLiteral("Selenized Light");
    if (id == QLatin1String("dracula")) return QStringLiteral("Dracula");
    if (id == QLatin1String("tokyo_night")) return QStringLiteral("Tokyo Night");
    if (id == QLatin1String("nord")) return QStringLiteral("Nord");
    if (id == QLatin1String("gruvbox")) return QStringLiteral("Gruvbox Dark");
    if (id == QLatin1String("one_dark")) return QStringLiteral("One Dark");
    return id;
}

void themeRowColors(const QString &id, QString *activeFg, QString *rebootFg,
                    bool darkBackground)
{
    if (!activeFg || !rebootFg) return;
    if (id == QLatin1String("system")) {
        // Tons legíveis tanto sobre fundo claro quanto sobre fundo escuro.
        *activeFg = QLatin1String(darkBackground ? "#a6d189" : "#2e7d32");
        *rebootFg = QLatin1String(darkBackground ? "#ef9f76" : "#e65100");
        return;
    }
    if (id == QLatin1String("octopkg")) { *activeFg = QStringLiteral("#2e7d32"); *rebootFg = QStringLiteral("#e65100"); return; }
    if (id == QLatin1String("frappe")) { *activeFg = QStringLiteral("#a6d189"); *rebootFg = QStringLiteral("#ef9f76"); return; }
    if (id == QLatin1String("latte")) { *activeFg = QStringLiteral("#40a02b"); *rebootFg = QStringLiteral("#fe640b"); return; }
    if (id == QLatin1String("mocha")) { *activeFg = QStringLiteral("#a6e3a1"); *rebootFg = QStringLiteral("#fab387"); return; }
    if (id == QLatin1String("macchiato")) { *activeFg = QStringLiteral("#a6da95"); *rebootFg = QStringLiteral("#f5a97f"); return; }
    if (id == QLatin1String("solarized_dark")) { *activeFg = QStringLiteral("#859900"); *rebootFg = QStringLiteral("#cb4b16"); return; }
    if (id == QLatin1String("solarized_light")) { *activeFg = QStringLiteral("#859900"); *rebootFg = QStringLiteral("#cb4b16"); return; }
    if (id == QLatin1String("selenized_dark")) { *activeFg = QStringLiteral("#75b938"); *rebootFg = QStringLiteral("#ed8649"); return; }
    if (id == QLatin1String("selenized_light")) { *activeFg = QStringLiteral("#489100"); *rebootFg = QStringLiteral("#bc6800"); return; }
    if (id == QLatin1String("dracula")) { *activeFg = QStringLiteral("#50fa7b"); *rebootFg = QStringLiteral("#ffb86c"); return; }
    if (id == QLatin1String("tokyo_night")) { *activeFg = QStringLiteral("#9ece6a"); *rebootFg = QStringLiteral("#ff9e64"); return; }
    if (id == QLatin1String("nord")) { *activeFg = QStringLiteral("#a3be8c"); *rebootFg = QStringLiteral("#d08770"); return; }
    if (id == QLatin1String("gruvbox")) { *activeFg = QStringLiteral("#b8bb26"); *rebootFg = QStringLiteral("#fe8019"); return; }
    if (id == QLatin1String("one_dark")) { *activeFg = QStringLiteral("#98c379"); *rebootFg = QStringLiteral("#d19a66"); return; }
    *activeFg = QStringLiteral("#2e7d32"); *rebootFg = QStringLiteral("#e65100");
}

QString themeStyleSheet(const QString &id)
{
    if (id == QLatin1String("system"))
        return QString();
    if (id == QLatin1String("octopkg")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #f5f6f8; color: #1a1d23; }
QMenuBar { background: #eef0f3; color: #1a1d23; }
QMenuBar::item:selected { background: #ffffff; }
QMenu { background: #eef0f3; color: #1a1d23; border: 1px solid #d0d4da; }
QMenu::item:selected { background: #3d7eac; color: #ffffff; }
QToolBar { background: #eef0f3; border-bottom: 1px solid #d0d4da; spacing: 6px; padding: 4px; }
QToolButton { color: #1a1d23; padding: 6px 10px; }
QToolButton:hover { background: #ffffff; }
QTableWidget { background: #ffffff;
  color: #1a1d23; gridline-color: #d0d4da;
  selection-background-color: #3d7eac; selection-color: #ffffff;
  alternate-background-color: #ffffff; }
QHeaderView::section { background: #e8ebf0; color: #1a1d23; padding: 6px; border: none;
  border-right: 1px solid #d0d4da; border-bottom: 1px solid #d0d4da; font-weight: 600; }
QTabWidget::pane { border: 1px solid #d0d4da; background: #eef0f3; }
QTabBar::tab { background: #ffffff; color: #1a1d23; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #eef0f3; border-bottom: 2px solid #3d7eac; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #d0d4da; border-radius: 3px;
  background: #ffffff; color: #1a1d23; }
QStatusBar { background: #eef0f3; color: #5c6370; border-top: 1px solid #d0d4da; }
QTextEdit { background: #eef0f3; color: #1a1d23; border: none; font-family: monospace; }
QLabel { color: #1a1d23; }
QPushButton { background: #ffffff; color: #1a1d23; border: 1px solid #d0d4da; padding: 6px 12px; }
QPushButton:hover { background: #e8ebf0; }
QCheckBox { color: #1a1d23; }
QMessageBox { background: #f5f6f8; color: #1a1d23; })QSS");
    }
    if (id == QLatin1String("frappe")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #303446; color: #c6d0f5; }
QMenuBar { background: #292c3c; color: #c6d0f5; }
QMenuBar::item:selected { background: #414559; }
QMenu { background: #292c3c; color: #c6d0f5; border: 1px solid #51576d; }
QMenu::item:selected { background: #8caaee; color: #303446; }
QToolBar { background: #292c3c; border-bottom: 1px solid #51576d; spacing: 6px; padding: 4px; }
QToolButton { color: #c6d0f5; padding: 6px 10px; }
QToolButton:hover { background: #414559; }
QTableWidget { background: #292c3c;
  color: #c6d0f5; gridline-color: #51576d;
  selection-background-color: #8caaee; selection-color: #303446;
  alternate-background-color: #414559; }
QHeaderView::section { background: #51576d; color: #c6d0f5; padding: 6px; border: none;
  border-right: 1px solid #51576d; border-bottom: 1px solid #51576d; font-weight: 600; }
QTabWidget::pane { border: 1px solid #51576d; background: #292c3c; }
QTabBar::tab { background: #414559; color: #c6d0f5; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #292c3c; border-bottom: 2px solid #8caaee; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #51576d; border-radius: 3px;
  background: #414559; color: #c6d0f5; }
QStatusBar { background: #292c3c; color: #a5adce; border-top: 1px solid #51576d; }
QTextEdit { background: #292c3c; color: #c6d0f5; border: none; font-family: monospace; }
QLabel { color: #c6d0f5; }
QPushButton { background: #414559; color: #c6d0f5; border: 1px solid #51576d; padding: 6px 12px; }
QPushButton:hover { background: #51576d; }
QCheckBox { color: #c6d0f5; }
QMessageBox { background: #303446; color: #c6d0f5; })QSS");
    }
    if (id == QLatin1String("latte")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #eff1f5; color: #4c4f69; }
QMenuBar { background: #e6e9ef; color: #4c4f69; }
QMenuBar::item:selected { background: #ccd0da; }
QMenu { background: #e6e9ef; color: #4c4f69; border: 1px solid #bcc0cc; }
QMenu::item:selected { background: #1e66f5; color: #eff1f5; }
QToolBar { background: #e6e9ef; border-bottom: 1px solid #bcc0cc; spacing: 6px; padding: 4px; }
QToolButton { color: #4c4f69; padding: 6px 10px; }
QToolButton:hover { background: #ccd0da; }
QTableWidget { background: #ccd0da;
  color: #4c4f69; gridline-color: #bcc0cc;
  selection-background-color: #1e66f5; selection-color: #eff1f5;
  alternate-background-color: #ccd0da; }
QHeaderView::section { background: #bcc0cc; color: #4c4f69; padding: 6px; border: none;
  border-right: 1px solid #bcc0cc; border-bottom: 1px solid #bcc0cc; font-weight: 600; }
QTabWidget::pane { border: 1px solid #bcc0cc; background: #e6e9ef; }
QTabBar::tab { background: #ccd0da; color: #4c4f69; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #e6e9ef; border-bottom: 2px solid #1e66f5; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #bcc0cc; border-radius: 3px;
  background: #ccd0da; color: #4c4f69; }
QStatusBar { background: #e6e9ef; color: #6c6f85; border-top: 1px solid #bcc0cc; }
QTextEdit { background: #e6e9ef; color: #4c4f69; border: none; font-family: monospace; }
QLabel { color: #4c4f69; }
QPushButton { background: #ccd0da; color: #4c4f69; border: 1px solid #bcc0cc; padding: 6px 12px; }
QPushButton:hover { background: #bcc0cc; }
QCheckBox { color: #4c4f69; }
QMessageBox { background: #eff1f5; color: #4c4f69; })QSS");
    }
    if (id == QLatin1String("mocha")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #1e1e2e; color: #cdd6f4; }
QMenuBar { background: #181825; color: #cdd6f4; }
QMenuBar::item:selected { background: #313244; }
QMenu { background: #181825; color: #cdd6f4; border: 1px solid #45475a; }
QMenu::item:selected { background: #89b4fa; color: #1e1e2e; }
QToolBar { background: #181825; border-bottom: 1px solid #45475a; spacing: 6px; padding: 4px; }
QToolButton { color: #cdd6f4; padding: 6px 10px; }
QToolButton:hover { background: #313244; }
QTableWidget { background: #181825;
  color: #cdd6f4; gridline-color: #45475a;
  selection-background-color: #89b4fa; selection-color: #1e1e2e;
  alternate-background-color: #313244; }
QHeaderView::section { background: #45475a; color: #cdd6f4; padding: 6px; border: none;
  border-right: 1px solid #45475a; border-bottom: 1px solid #45475a; font-weight: 600; }
QTabWidget::pane { border: 1px solid #45475a; background: #181825; }
QTabBar::tab { background: #313244; color: #cdd6f4; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #181825; border-bottom: 2px solid #89b4fa; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #45475a; border-radius: 3px;
  background: #313244; color: #cdd6f4; }
QStatusBar { background: #181825; color: #a6adc8; border-top: 1px solid #45475a; }
QTextEdit { background: #181825; color: #cdd6f4; border: none; font-family: monospace; }
QLabel { color: #cdd6f4; }
QPushButton { background: #313244; color: #cdd6f4; border: 1px solid #45475a; padding: 6px 12px; }
QPushButton:hover { background: #45475a; }
QCheckBox { color: #cdd6f4; }
QMessageBox { background: #1e1e2e; color: #cdd6f4; })QSS");
    }
    if (id == QLatin1String("macchiato")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #24273a; color: #cad3f5; }
QMenuBar { background: #1e2030; color: #cad3f5; }
QMenuBar::item:selected { background: #363a4f; }
QMenu { background: #1e2030; color: #cad3f5; border: 1px solid #494d64; }
QMenu::item:selected { background: #8aadf4; color: #24273a; }
QToolBar { background: #1e2030; border-bottom: 1px solid #494d64; spacing: 6px; padding: 4px; }
QToolButton { color: #cad3f5; padding: 6px 10px; }
QToolButton:hover { background: #363a4f; }
QTableWidget { background: #1e2030;
  color: #cad3f5; gridline-color: #494d64;
  selection-background-color: #8aadf4; selection-color: #24273a;
  alternate-background-color: #363a4f; }
QHeaderView::section { background: #494d64; color: #cad3f5; padding: 6px; border: none;
  border-right: 1px solid #494d64; border-bottom: 1px solid #494d64; font-weight: 600; }
QTabWidget::pane { border: 1px solid #494d64; background: #1e2030; }
QTabBar::tab { background: #363a4f; color: #cad3f5; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #1e2030; border-bottom: 2px solid #8aadf4; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #494d64; border-radius: 3px;
  background: #363a4f; color: #cad3f5; }
QStatusBar { background: #1e2030; color: #a5adcb; border-top: 1px solid #494d64; }
QTextEdit { background: #1e2030; color: #cad3f5; border: none; font-family: monospace; }
QLabel { color: #cad3f5; }
QPushButton { background: #363a4f; color: #cad3f5; border: 1px solid #494d64; padding: 6px 12px; }
QPushButton:hover { background: #494d64; }
QCheckBox { color: #cad3f5; }
QMessageBox { background: #24273a; color: #cad3f5; })QSS");
    }
    if (id == QLatin1String("solarized_dark")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #002b36; color: #839496; }
QMenuBar { background: #073642; color: #839496; }
QMenuBar::item:selected { background: #073642; }
QMenu { background: #073642; color: #839496; border: 1px solid #586e75; }
QMenu::item:selected { background: #268bd2; color: #002b36; }
QToolBar { background: #073642; border-bottom: 1px solid #586e75; spacing: 6px; padding: 4px; }
QToolButton { color: #839496; padding: 6px 10px; }
QToolButton:hover { background: #073642; }
QTableWidget { background: #073642;
  color: #839496; gridline-color: #586e75;
  selection-background-color: #268bd2; selection-color: #002b36;
  alternate-background-color: #073642; }
QHeaderView::section { background: #586e75; color: #839496; padding: 6px; border: none;
  border-right: 1px solid #586e75; border-bottom: 1px solid #586e75; font-weight: 600; }
QTabWidget::pane { border: 1px solid #586e75; background: #073642; }
QTabBar::tab { background: #073642; color: #839496; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #073642; border-bottom: 2px solid #268bd2; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #586e75; border-radius: 3px;
  background: #073642; color: #839496; }
QStatusBar { background: #073642; color: #657b83; border-top: 1px solid #586e75; }
QTextEdit { background: #073642; color: #839496; border: none; font-family: monospace; }
QLabel { color: #839496; }
QPushButton { background: #073642; color: #839496; border: 1px solid #586e75; padding: 6px 12px; }
QPushButton:hover { background: #586e75; }
QCheckBox { color: #839496; }
QMessageBox { background: #002b36; color: #839496; })QSS");
    }
    if (id == QLatin1String("solarized_light")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #fdf6e3; color: #657b83; }
QMenuBar { background: #eee8d5; color: #657b83; }
QMenuBar::item:selected { background: #eee8d5; }
QMenu { background: #eee8d5; color: #657b83; border: 1px solid #93a1a1; }
QMenu::item:selected { background: #268bd2; color: #fdf6e3; }
QToolBar { background: #eee8d5; border-bottom: 1px solid #93a1a1; spacing: 6px; padding: 4px; }
QToolButton { color: #657b83; padding: 6px 10px; }
QToolButton:hover { background: #eee8d5; }
QTableWidget { background: #eee8d5;
  color: #657b83; gridline-color: #93a1a1;
  selection-background-color: #268bd2; selection-color: #fdf6e3;
  alternate-background-color: #eee8d5; }
QHeaderView::section { background: #93a1a1; color: #657b83; padding: 6px; border: none;
  border-right: 1px solid #93a1a1; border-bottom: 1px solid #93a1a1; font-weight: 600; }
QTabWidget::pane { border: 1px solid #93a1a1; background: #eee8d5; }
QTabBar::tab { background: #eee8d5; color: #657b83; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #eee8d5; border-bottom: 2px solid #268bd2; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #93a1a1; border-radius: 3px;
  background: #eee8d5; color: #657b83; }
QStatusBar { background: #eee8d5; color: #93a1a1; border-top: 1px solid #93a1a1; }
QTextEdit { background: #eee8d5; color: #657b83; border: none; font-family: monospace; }
QLabel { color: #657b83; }
QPushButton { background: #eee8d5; color: #657b83; border: 1px solid #93a1a1; padding: 6px 12px; }
QPushButton:hover { background: #93a1a1; }
QCheckBox { color: #657b83; }
QMessageBox { background: #fdf6e3; color: #657b83; })QSS");
    }
    if (id == QLatin1String("selenized_dark")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #103c48; color: #adbcbc; }
QMenuBar { background: #184956; color: #adbcbc; }
QMenuBar::item:selected { background: #2d5b69; }
QMenu { background: #184956; color: #adbcbc; border: 1px solid #72898f; }
QMenu::item:selected { background: #4695f7; color: #103c48; }
QToolBar { background: #184956; border-bottom: 1px solid #72898f; spacing: 6px; padding: 4px; }
QToolButton { color: #adbcbc; padding: 6px 10px; }
QToolButton:hover { background: #2d5b69; }
QTableWidget { background: #184956;
  color: #adbcbc; gridline-color: #72898f;
  selection-background-color: #4695f7; selection-color: #103c48;
  alternate-background-color: #2d5b69; }
QHeaderView::section { background: #72898f; color: #adbcbc; padding: 6px; border: none;
  border-right: 1px solid #72898f; border-bottom: 1px solid #72898f; font-weight: 600; }
QTabWidget::pane { border: 1px solid #72898f; background: #184956; }
QTabBar::tab { background: #2d5b69; color: #adbcbc; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #184956; border-bottom: 2px solid #4695f7; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #72898f; border-radius: 3px;
  background: #2d5b69; color: #adbcbc; }
QStatusBar { background: #184956; color: #72898f; border-top: 1px solid #72898f; }
QTextEdit { background: #184956; color: #adbcbc; border: none; font-family: monospace; }
QLabel { color: #adbcbc; }
QPushButton { background: #2d5b69; color: #adbcbc; border: 1px solid #72898f; padding: 6px 12px; }
QPushButton:hover { background: #72898f; }
QCheckBox { color: #adbcbc; }
QMessageBox { background: #103c48; color: #adbcbc; })QSS");
    }
    if (id == QLatin1String("selenized_light")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #fbf3db; color: #53676d; }
QMenuBar { background: #ece3cc; color: #53676d; }
QMenuBar::item:selected { background: #e9e4d0; }
QMenu { background: #ece3cc; color: #53676d; border: 1px solid #909995; }
QMenu::item:selected { background: #0072d4; color: #fbf3db; }
QToolBar { background: #ece3cc; border-bottom: 1px solid #909995; spacing: 6px; padding: 4px; }
QToolButton { color: #53676d; padding: 6px 10px; }
QToolButton:hover { background: #e9e4d0; }
QTableWidget { background: #e9e4d0;
  color: #53676d; gridline-color: #909995;
  selection-background-color: #0072d4; selection-color: #fbf3db;
  alternate-background-color: #e9e4d0; }
QHeaderView::section { background: #909995; color: #53676d; padding: 6px; border: none;
  border-right: 1px solid #909995; border-bottom: 1px solid #909995; font-weight: 600; }
QTabWidget::pane { border: 1px solid #909995; background: #ece3cc; }
QTabBar::tab { background: #e9e4d0; color: #53676d; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #ece3cc; border-bottom: 2px solid #0072d4; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #909995; border-radius: 3px;
  background: #e9e4d0; color: #53676d; }
QStatusBar { background: #ece3cc; color: #909995; border-top: 1px solid #909995; }
QTextEdit { background: #ece3cc; color: #53676d; border: none; font-family: monospace; }
QLabel { color: #53676d; }
QPushButton { background: #e9e4d0; color: #53676d; border: 1px solid #909995; padding: 6px 12px; }
QPushButton:hover { background: #909995; }
QCheckBox { color: #53676d; }
QMessageBox { background: #fbf3db; color: #53676d; })QSS");
    }
    if (id == QLatin1String("dracula")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #282a36; color: #f8f8f2; }
QMenuBar { background: #21222c; color: #f8f8f2; }
QMenuBar::item:selected { background: #44475a; }
QMenu { background: #21222c; color: #f8f8f2; border: 1px solid #6272a4; }
QMenu::item:selected { background: #bd93f9; color: #282a36; }
QToolBar { background: #21222c; border-bottom: 1px solid #6272a4; spacing: 6px; padding: 4px; }
QToolButton { color: #f8f8f2; padding: 6px 10px; }
QToolButton:hover { background: #44475a; }
QTableWidget { background: #21222c;
  color: #f8f8f2; gridline-color: #6272a4;
  selection-background-color: #bd93f9; selection-color: #282a36;
  alternate-background-color: #44475a; }
QHeaderView::section { background: #6272a4; color: #f8f8f2; padding: 6px; border: none;
  border-right: 1px solid #6272a4; border-bottom: 1px solid #6272a4; font-weight: 600; }
QTabWidget::pane { border: 1px solid #6272a4; background: #21222c; }
QTabBar::tab { background: #44475a; color: #f8f8f2; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #21222c; border-bottom: 2px solid #bd93f9; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #6272a4; border-radius: 3px;
  background: #44475a; color: #f8f8f2; }
QStatusBar { background: #21222c; color: #6272a4; border-top: 1px solid #6272a4; }
QTextEdit { background: #21222c; color: #f8f8f2; border: none; font-family: monospace; }
QLabel { color: #f8f8f2; }
QPushButton { background: #44475a; color: #f8f8f2; border: 1px solid #6272a4; padding: 6px 12px; }
QPushButton:hover { background: #6272a4; }
QCheckBox { color: #f8f8f2; }
QMessageBox { background: #282a36; color: #f8f8f2; })QSS");
    }
    if (id == QLatin1String("tokyo_night")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #1a1b26; color: #c0caf5; }
QMenuBar { background: #16161e; color: #c0caf5; }
QMenuBar::item:selected { background: #24283b; }
QMenu { background: #16161e; color: #c0caf5; border: 1px solid #414868; }
QMenu::item:selected { background: #7aa2f7; color: #1a1b26; }
QToolBar { background: #16161e; border-bottom: 1px solid #414868; spacing: 6px; padding: 4px; }
QToolButton { color: #c0caf5; padding: 6px 10px; }
QToolButton:hover { background: #24283b; }
QTableWidget { background: #16161e;
  color: #c0caf5; gridline-color: #414868;
  selection-background-color: #7aa2f7; selection-color: #1a1b26;
  alternate-background-color: #24283b; }
QHeaderView::section { background: #414868; color: #c0caf5; padding: 6px; border: none;
  border-right: 1px solid #414868; border-bottom: 1px solid #414868; font-weight: 600; }
QTabWidget::pane { border: 1px solid #414868; background: #16161e; }
QTabBar::tab { background: #24283b; color: #c0caf5; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #16161e; border-bottom: 2px solid #7aa2f7; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #414868; border-radius: 3px;
  background: #24283b; color: #c0caf5; }
QStatusBar { background: #16161e; color: #565f89; border-top: 1px solid #414868; }
QTextEdit { background: #16161e; color: #c0caf5; border: none; font-family: monospace; }
QLabel { color: #c0caf5; }
QPushButton { background: #24283b; color: #c0caf5; border: 1px solid #414868; padding: 6px 12px; }
QPushButton:hover { background: #414868; }
QCheckBox { color: #c0caf5; }
QMessageBox { background: #1a1b26; color: #c0caf5; })QSS");
    }
    if (id == QLatin1String("nord")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #2e3440; color: #d8dee9; }
QMenuBar { background: #3b4252; color: #d8dee9; }
QMenuBar::item:selected { background: #434c5e; }
QMenu { background: #3b4252; color: #d8dee9; border: 1px solid #4c566a; }
QMenu::item:selected { background: #81a1c1; color: #2e3440; }
QToolBar { background: #3b4252; border-bottom: 1px solid #4c566a; spacing: 6px; padding: 4px; }
QToolButton { color: #d8dee9; padding: 6px 10px; }
QToolButton:hover { background: #434c5e; }
QTableWidget { background: #3b4252;
  color: #d8dee9; gridline-color: #4c566a;
  selection-background-color: #81a1c1; selection-color: #2e3440;
  alternate-background-color: #434c5e; }
QHeaderView::section { background: #4c566a; color: #d8dee9; padding: 6px; border: none;
  border-right: 1px solid #4c566a; border-bottom: 1px solid #4c566a; font-weight: 600; }
QTabWidget::pane { border: 1px solid #4c566a; background: #3b4252; }
QTabBar::tab { background: #434c5e; color: #d8dee9; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #3b4252; border-bottom: 2px solid #81a1c1; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #4c566a; border-radius: 3px;
  background: #434c5e; color: #d8dee9; }
QStatusBar { background: #3b4252; color: #4c566a; border-top: 1px solid #4c566a; }
QTextEdit { background: #3b4252; color: #d8dee9; border: none; font-family: monospace; }
QLabel { color: #d8dee9; }
QPushButton { background: #434c5e; color: #d8dee9; border: 1px solid #4c566a; padding: 6px 12px; }
QPushButton:hover { background: #4c566a; }
QCheckBox { color: #d8dee9; }
QMessageBox { background: #2e3440; color: #d8dee9; })QSS");
    }
    if (id == QLatin1String("gruvbox")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #282828; color: #ebdbb2; }
QMenuBar { background: #1d2021; color: #ebdbb2; }
QMenuBar::item:selected { background: #3c3836; }
QMenu { background: #1d2021; color: #ebdbb2; border: 1px solid #504945; }
QMenu::item:selected { background: #83a598; color: #282828; }
QToolBar { background: #1d2021; border-bottom: 1px solid #504945; spacing: 6px; padding: 4px; }
QToolButton { color: #ebdbb2; padding: 6px 10px; }
QToolButton:hover { background: #3c3836; }
QTableWidget { background: #1d2021;
  color: #ebdbb2; gridline-color: #504945;
  selection-background-color: #83a598; selection-color: #282828;
  alternate-background-color: #3c3836; }
QHeaderView::section { background: #504945; color: #ebdbb2; padding: 6px; border: none;
  border-right: 1px solid #504945; border-bottom: 1px solid #504945; font-weight: 600; }
QTabWidget::pane { border: 1px solid #504945; background: #1d2021; }
QTabBar::tab { background: #3c3836; color: #ebdbb2; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #1d2021; border-bottom: 2px solid #83a598; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #504945; border-radius: 3px;
  background: #3c3836; color: #ebdbb2; }
QStatusBar { background: #1d2021; color: #a89984; border-top: 1px solid #504945; }
QTextEdit { background: #1d2021; color: #ebdbb2; border: none; font-family: monospace; }
QLabel { color: #ebdbb2; }
QPushButton { background: #3c3836; color: #ebdbb2; border: 1px solid #504945; padding: 6px 12px; }
QPushButton:hover { background: #504945; }
QCheckBox { color: #ebdbb2; }
QMessageBox { background: #282828; color: #ebdbb2; })QSS");
    }
    if (id == QLatin1String("one_dark")) {
        return QString(R"QSS(QMainWindow, QDialog { background: #282c34; color: #abb2bf; }
QMenuBar { background: #21252b; color: #abb2bf; }
QMenuBar::item:selected { background: #3b4048; }
QMenu { background: #21252b; color: #abb2bf; border: 1px solid #4b5263; }
QMenu::item:selected { background: #61afef; color: #282c34; }
QToolBar { background: #21252b; border-bottom: 1px solid #4b5263; spacing: 6px; padding: 4px; }
QToolButton { color: #abb2bf; padding: 6px 10px; }
QToolButton:hover { background: #3b4048; }
QTableWidget { background: #21252b;
  color: #abb2bf; gridline-color: #4b5263;
  selection-background-color: #61afef; selection-color: #282c34;
  alternate-background-color: #3b4048; }
QHeaderView::section { background: #4b5263; color: #abb2bf; padding: 6px; border: none;
  border-right: 1px solid #4b5263; border-bottom: 1px solid #4b5263; font-weight: 600; }
QTabWidget::pane { border: 1px solid #4b5263; background: #21252b; }
QTabBar::tab { background: #3b4048; color: #abb2bf; padding: 8px 16px; margin-right: 2px; }
QTabBar::tab:selected { background: #21252b; border-bottom: 2px solid #61afef; }
QLineEdit, QComboBox, QSpinBox { padding: 6px 8px; border: 1px solid #4b5263; border-radius: 3px;
  background: #3b4048; color: #abb2bf; }
QStatusBar { background: #21252b; color: #5c6370; border-top: 1px solid #4b5263; }
QTextEdit { background: #21252b; color: #abb2bf; border: none; font-family: monospace; }
QLabel { color: #abb2bf; }
QPushButton { background: #3b4048; color: #abb2bf; border: 1px solid #4b5263; padding: 6px 12px; }
QPushButton:hover { background: #4b5263; }
QCheckBox { color: #abb2bf; }
QMessageBox { background: #282c34; color: #abb2bf; })QSS");
    }
    return themeStyleSheet(QStringLiteral("octopkg"));
}