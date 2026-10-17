// Teste da interface com um bectl falso. Roda em qualquer sistema, com a
// plataforma offscreen. Verifica que a janela continua respondendo enquanto
// o bectl roda, que filtro e seleção usam a lista em cache e que as ações
// acompanham a seleção e o estado de ocupado.

#include "mainwindow.h"
#include "i18n.h"

#include <QAction>
#include <QApplication>
#include <QElapsedTimer>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QTableWidget>
#include <QTemporaryDir>
#include <QtTest>

class TestMainWindow : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_dir;
    QString m_bectl;

    QString logPath() const { return m_dir.filePath(QStringLiteral("calls.log")); }

    int listCalls() const
    {
        QFile f(logPath());
        if (!f.open(QIODevice::ReadOnly))
            return 0;
        return QString::fromUtf8(f.readAll()).count(QStringLiteral("list -H"));
    }

    static QAction *action(MainWindow &w, const char *name)
    {
        auto *a = w.findChild<QAction *>(QLatin1String(name));
        if (!a)
            qFatal("ação %s não encontrada", name);
        return a;
    }

    static void selectRows(QTableWidget *t, const QList<int> &rows)
    {
        t->clearSelection();
        for (int r : rows)
            t->selectionModel()->select(t->model()->index(r, 0),
                                        QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }

private slots:
    void initTestCase()
    {
        I18n::setLocale(QStringLiteral("en_US"));
        // "list" demora 1 s, para dar tempo de verificar a janela durante a execução.
        m_bectl = m_dir.filePath(QStringLiteral("bectl"));
        QFile f(m_bectl);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write(QStringLiteral(
                    "#!/bin/sh\n"
                    "echo \"$*\" >> '%1'\n"
                    "if [ \"$1\" = list ]; then\n"
                    "  sleep 1\n"
                    "  printf 'default\\tNR\\t/\\t1.2G\\t2026-10-01 10:00\\n'\n"
                    "  printf 'be-mounted\\t-\\t/mnt/be\\t10M\\t2026-10-02 11:00\\n'\n"
                    "  printf 'be-idle\\t-\\t-\\t5M\\t2026-10-03 12:00\\n'\n"
                    "fi\n")
                    .arg(logPath())
                    .toUtf8());
        f.close();
        f.setPermissions(f.permissions() | QFileDevice::ExeOwner);
    }

    void init() { QFile::remove(logPath()); }

    void staysResponsiveWhileListing()
    {
        QElapsedTimer timer;
        timer.start();
        MainWindow w(nullptr, m_bectl, false);
        // O construtor dispara a listagem, mas não espera o bectl (que leva 1 s).
        QVERIFY2(timer.elapsed() < 800, qPrintable(QString::number(timer.elapsed())));
        QVERIFY(!action(w, "actRefresh")->isEnabled());
        QCOMPARE(w.findChild<QLabel *>(QStringLiteral("status"))->text(),
                 QStringLiteral("Listing Boot Environments…"));

        auto *table = w.findChild<QTableWidget *>(QStringLiteral("table"));
        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), 3, 5000);
        QVERIFY(action(w, "actRefresh")->isEnabled());
        QCOMPARE(listCalls(), 1);
    }

    void filterAndSelectionUseCache()
    {
        MainWindow w(nullptr, m_bectl, false);
        auto *table = w.findChild<QTableWidget *>(QStringLiteral("table"));
        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), 3, 5000);

        w.findChild<QLineEdit *>(QStringLiteral("filter"))->setText(QStringLiteral("mount"));
        QCOMPARE(table->rowCount(), 1);
        selectRows(table, {0});
        w.findChild<QLineEdit *>(QStringLiteral("filter"))->clear();
        QCOMPARE(table->rowCount(), 3);
        QCOMPARE(listCalls(), 1);
    }

    void actionsFollowSelection()
    {
        MainWindow w(nullptr, m_bectl, false);
        auto *table = w.findChild<QTableWidget *>(QStringLiteral("table"));
        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), 3, 5000);

        selectRows(table, {0});   // default: BE em uso, montado em "/"
        QVERIFY(!action(w, "actMount")->isEnabled());
        QVERIFY(!action(w, "actUnmount")->isEnabled());
        QVERIFY(action(w, "actActivate")->isEnabled());

        selectRows(table, {1});   // be-mounted
        QVERIFY(!action(w, "actMount")->isEnabled());
        QVERIFY(action(w, "actUnmount")->isEnabled());

        selectRows(table, {2});   // be-idle: não montado
        QVERIFY(action(w, "actMount")->isEnabled());
        QVERIFY(!action(w, "actUnmount")->isEnabled());

        selectRows(table, {0, 1});
        QVERIFY(!action(w, "actActivate")->isEnabled());
        QVERIFY(!action(w, "actRename")->isEnabled());
        QVERIFY(action(w, "actDestroy")->isEnabled());
    }

    void busyBlocksOtherOperationsAndClose()
    {
        MainWindow w(nullptr, m_bectl, false);
        auto *table = w.findChild<QTableWidget *>(QStringLiteral("table"));
        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), 3, 5000);
        selectRows(table, {1});

        action(w, "actRefresh")->trigger();
        for (const char *name : {"actRefresh", "actCreate", "actSnapshot", "actUnmount", "actDestroy"})
            QVERIFY2(!action(w, name)->isEnabled(), name);
        action(w, "actRefresh")->trigger();   // ignorado: a ação está desativada
        QVERIFY(!w.close());

        QTRY_VERIFY_WITH_TIMEOUT(action(w, "actRefresh")->isEnabled(), 5000);
        QCOMPARE(listCalls(), 2);
        QVERIFY(w.close());
    }

    void unmountRunsBectl()
    {
        MainWindow w(nullptr, m_bectl, false);
        auto *table = w.findChild<QTableWidget *>(QStringLiteral("table"));
        QTRY_COMPARE_WITH_TIMEOUT(table->rowCount(), 3, 5000);
        selectRows(table, {1});
        action(w, "actUnmount")->trigger();
        // umount e depois a nova listagem.
        QTRY_COMPARE_WITH_TIMEOUT(listCalls(), 2, 5000);
        QFile f(logPath());
        QVERIFY(f.open(QIODevice::ReadOnly));
        QVERIFY(QString::fromUtf8(f.readAll()).contains(QStringLiteral("umount be-mounted\n")));
        QTRY_VERIFY_WITH_TIMEOUT(action(w, "actRefresh")->isEnabled(), 5000);
    }
};

int main(int argc, char **argv)
{
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
        qputenv("QT_QPA_PLATFORM", "offscreen");
    QApplication app(argc, argv);
    TestMainWindow t;
    return QTest::qExec(&t, argc, argv);
}

#include "tst_mainwindow.moc"
