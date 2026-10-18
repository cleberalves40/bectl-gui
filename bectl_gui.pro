QT += core gui widgets
CONFIG += c++17

DEFINES += BECTL_GUI_VERSION=\\\"1.1.0\\\"

TARGET = bectl_gui
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/logic/bectlmanager.cpp \
    src/gui/mainwindow.cpp \
    src/gui/preferencesdialog.cpp \
    src/gui/themes.cpp \
    src/i18n/i18n.cpp

HEADERS += \
    src/logic/bectlmanager.h \
    src/gui/mainwindow.h \
    src/gui/preferencesdialog.h \
    src/gui/themes.h \
    src/i18n/i18n.h

INCLUDEPATH += \
    src \
    src/logic \
    src/gui \
    src/i18n

target.path = /usr/local/bin
desktop.path = /usr/local/share/applications
desktop.files = bectl_gui.desktop
INSTALLS += target desktop
