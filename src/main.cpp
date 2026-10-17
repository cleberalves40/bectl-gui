#include "mainwindow.h"
#include "i18n.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("bectl"));
    QApplication::setOrganizationName(QStringLiteral("FreeBSD"));
    QApplication::setApplicationVersion(QStringLiteral(BECTL_GUI_VERSION));

    /* Idioma do sistema; fallback en_US */
    I18n::initFromSystem();

    MainWindow w;
    w.show();
    return app.exec();
}
