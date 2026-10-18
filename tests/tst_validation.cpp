// Teste unitário das validações e mensagens do BectlManager. Roda em
// qualquer sistema: usa um bectl falso que só registra os argumentos
// recebidos, para verificar o que chegaria ao bectl(8) real.

#include "bectlmanager.h"
#include "i18n.h"

#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include <functional>
#include <stdexcept>

class TestValidation : public QObject
{
    Q_OBJECT

private:
    QTemporaryDir m_dir;

    QString logPath() const { return m_dir.filePath(QStringLiteral("calls.log")); }

    // Script que registra os argumentos e termina com o status pedido.
    QString fakeBectl(int status)
    {
        const QString path = m_dir.filePath(QStringLiteral("bectl-%1").arg(status));
        QFile f(path);
        if (!f.exists()) {
            if (!f.open(QIODevice::WriteOnly))
                qFatal("não foi possível criar %s", qPrintable(path));
            f.write(QStringLiteral("#!/bin/sh\necho \"$*\" >> '%1'\nexit %2\n")
                        .arg(logPath()).arg(status).toUtf8());
            f.close();
            f.setPermissions(f.permissions() | QFileDevice::ExeOwner);
        }
        return path;
    }

    QStringList calls() const
    {
        QFile f(logPath());
        if (!f.open(QIODevice::ReadOnly))
            return {};
        return QString::fromUtf8(f.readAll()).split(QLatin1Char('\n'), Qt::SkipEmptyParts);
    }

    // Executa op esperando exceção e sem que o bectl seja chamado.
    QString rejected(const std::function<void()> &op)
    {
        QFile::remove(logPath());
        try {
            op();
        } catch (const std::runtime_error &e) {
            if (!calls().isEmpty())
                return QStringLiteral("bectl chamado: ") + calls().join(QLatin1Char('|'));
            return QString::fromUtf8(e.what());
        }
        return QStringLiteral("sem exceção");
    }

private slots:
    void init()
    {
        I18n::setLocale(QStringLiteral("en_US"));
        QFile::remove(logPath());
    }

    void rejectsInvalidNames_data()
    {
        QTest::addColumn<QString>("name");
        QTest::newRow("opção") << QStringLiteral("-F");
        QTest::newRow("opção longa") << QStringLiteral("--help");
        QTest::newRow("vazio") << QString();
        QTest::newRow("espaço") << QStringLiteral("meu be");
        QTest::newRow("barra") << QStringLiteral("pool/be");
        QTest::newRow("ponto") << QStringLiteral(".");
        QTest::newRow("dois pontos") << QStringLiteral("..");
        QTest::newRow("arroba") << QStringLiteral("be@snap");
        QTest::newRow("acento") << QStringLiteral("versão");
        QTest::newRow("longo") << QString(256, QLatin1Char('a'));
    }

    void rejectsInvalidNames()
    {
        QFETCH(QString, name);
        BectlManager mgr(false, QString(), fakeBectl(0));
        const QString prefix = QStringLiteral("Invalid boot environment name");
        QVERIFY(rejected([&] { mgr.create(name); }).startsWith(prefix));
        QVERIFY(rejected([&] { mgr.activate(name); }).startsWith(prefix));
        QVERIFY(rejected([&] { mgr.rename(QStringLiteral("default"), name); }).startsWith(prefix));
        QVERIFY(rejected([&] { mgr.rename(name, QStringLiteral("default")); }).startsWith(prefix));
        QVERIFY(rejected([&] { mgr.mount(name); }).startsWith(prefix));
        QVERIFY(rejected([&] { mgr.unmount(name); }).startsWith(prefix));
    }

    void rejectsInvalidSnapshots_data()
    {
        QTest::addColumn<QString>("name");
        QTest::newRow("sem nome") << QStringLiteral("default@");
        QTest::newRow("opção") << QStringLiteral("default@-r");
        QTest::newRow("dois arrobas") << QStringLiteral("a@b@c");
        QTest::newRow("be inválido") << QStringLiteral("-x@snap");
    }

    void rejectsInvalidSnapshots()
    {
        QFETCH(QString, name);
        BectlManager mgr(false, QString(), fakeBectl(0));
        QVERIFY(rejected([&] { mgr.createSnapshot(name); }).startsWith(QStringLiteral("Invalid")));
        QVERIFY(rejected([&] { mgr.create(QStringLiteral("novo"), name); }).startsWith(QStringLiteral("Invalid")));
        QVERIFY(rejected([&] { mgr.destroy(name); }).startsWith(QStringLiteral("Invalid")));
    }

    void rejectsRelativeMountPoint()
    {
        BectlManager mgr(false, QString(), fakeBectl(0));
        QVERIFY(rejected([&] { mgr.mount(QStringLiteral("default"), QStringLiteral("mnt")); })
                    .startsWith(QStringLiteral("Mount point must be an absolute path")));
    }

    void passesValidNames()
    {
        BectlManager mgr(false, QString(), fakeBectl(0));
        mgr.create(QStringLiteral("13.2-RELEASE_2026.10:a"), QStringLiteral("default@snap1"), true);
        mgr.createSnapshot(QStringLiteral("default@antes-upgrade"));
        mgr.activate(QStringLiteral("be1"), true);
        mgr.rename(QStringLiteral("be1"), QStringLiteral("be2"));
        mgr.mount(QStringLiteral("be2"), QStringLiteral("/mnt/be2"));
        mgr.unmount(QStringLiteral("be2"), true);
        mgr.destroy(QStringLiteral("be2@snap"), true, true);
        QCOMPARE(calls(), QStringList({
            QStringLiteral("create -r -e default@snap1 13.2-RELEASE_2026.10:a"),
            QStringLiteral("create default@antes-upgrade"),
            QStringLiteral("activate -t be1"),
            QStringLiteral("rename be1 be2"),
            QStringLiteral("mount be2 /mnt/be2"),
            QStringLiteral("umount -f be2"),
            QStringLiteral("destroy -F -o be2@snap"),
        }));
    }

    void reportsExitStatus()
    {
        BectlManager mgr(false, QString(), fakeBectl(3));
        try {
            mgr.activate(QStringLiteral("be1"));
            QFAIL("sem exceção");
        } catch (const std::runtime_error &e) {
            QCOMPARE(QString::fromUtf8(e.what()), QStringLiteral("Command failed with exit code 3"));
        }
    }

    void translatesMessages()
    {
        BectlManager mgr(false, QString(), fakeBectl(0));
        I18n::setLocale(QStringLiteral("pt_BR"));
        QCOMPARE(rejected([&] { mgr.create(QStringLiteral("-F")); }),
                 QStringLiteral("Nome de Boot Environment inválido: -F"));
        I18n::setLocale(QStringLiteral("es_ES"));
        QCOMPARE(rejected([&] { mgr.create(QStringLiteral("-F")); }),
                 QStringLiteral("Nombre de Boot Environment no válido: -F"));
    }

    // Formato de sbin/bectl/bectl_list.c com -H -s: BE sem recuo; dataset e
    // snapshots com dois espaços; origem de clone com quatro.
    void parsesSnapshotList()
    {
        const QString path = m_dir.filePath(QStringLiteral("bectl-snapshots"));
        QFile f(path);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("#!/bin/sh\n"
                "printf 'default\\n'\n"
                "printf '  zroot/ROOT/default\\tNR\\t/\\t1.2G\\t2026-10-01 10:00\\n'\n"
                "printf '  zroot/ROOT/default@antes-upgrade\\t-\\t-\\t300M\\t2026-10-05 09:30\\n'\n"
                "printf '  zroot/ROOT/default@2026-10-10-08:00:00-0\\t-\\t-\\t12K\\t2026-10-10 08:00\\n'\n"
                "printf '\\n'\n"
                "printf 'be-clone\\n'\n"
                "printf '  zroot/ROOT/be-clone\\t-\\t-\\t8K\\t2026-10-10 08:00\\n'\n"
                "printf '    zroot/ROOT/default@2026-10-10-08:00:00-0\\t-\\t-\\t12K\\t2026-10-10 08:00\\n'\n");
        f.close();
        f.setPermissions(f.permissions() | QFileDevice::ExeOwner);

        BectlManager mgr(false, QString(), path);
        const QVector<BeSnapshot> snaps = mgr.listSnapshots();
        QCOMPARE(snaps.size(), 2);
        QCOMPARE(snaps[0].name, QStringLiteral("default@antes-upgrade"));
        QCOMPARE(snaps[0].environment(), QStringLiteral("default"));
        QCOMPARE(snaps[0].space, QStringLiteral("300M"));
        QCOMPARE(snaps[0].created, QStringLiteral("2026-10-05 09:30"));
        QCOMPARE(snaps[1].name, QStringLiteral("default@2026-10-10-08:00:00-0"));
    }

    void missingQtSudoIsReported()
    {
        BectlManager mgr(true, QStringLiteral("qt-sudo-que-nao-existe"), fakeBectl(0));
        QVERIFY(rejected([&] { mgr.activate(QStringLiteral("be1")); })
                    .startsWith(QStringLiteral("'qt-sudo-que-nao-existe' not found")));
    }
};

QTEST_GUILESS_MAIN(TestValidation)
#include "tst_validation.moc"
