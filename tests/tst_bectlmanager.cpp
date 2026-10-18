// Teste de integração do BectlManager contra o bectl(8) real.
//
// Precisa de root e de um pool ZFS de rascunho organizado como raiz de boot
// environments. tests/freebsd-e2e.sh prepara esse pool e define
// BECTL_TEST_CMD com um wrapper que executa "bectl -r <pool>/ROOT", de modo
// que os boot environments do sistema nunca são tocados. Sem a variável, o
// teste é ignorado.

#include "bectlmanager.h"

#include <QDir>
#include <QTemporaryDir>
#include <QtTest>

#include <algorithm>
#include <stdexcept>

class TestBectlManager : public QObject
{
    Q_OBJECT

private:
    BectlManager m_mgr{false, QString(), qEnvironmentVariable("BECTL_TEST_CMD")};

    BootEnvironment be(const QString &name)
    {
        const auto found = m_mgr.getEnvironment(name);
        if (!found)
            qFatal("boot environment %s não encontrado", qPrintable(name));
        return *found;
    }

private slots:
    void initTestCase()
    {
        if (qEnvironmentVariableIsEmpty("BECTL_TEST_CMD"))
            QSKIP("BECTL_TEST_CMD não definido; execute via tests/freebsd-e2e.sh");
    }

    // O pool começa só com "default", marcado como ativo no próximo boot.
    void listParsesRealOutput()
    {
        const auto list = m_mgr.listEnvironments();
        QCOMPARE(list.size(), 1);
        const BootEnvironment &d = list.first();
        QCOMPARE(d.name, QStringLiteral("default"));
        QVERIFY2(d.isActiveReboot(), qPrintable(d.active));
        QVERIFY(!d.isActiveNow());
        QVERIFY(!d.isMounted());
        QVERIFY(!d.space.isEmpty());
        QVERIFY2(d.created.contains(QLatin1Char(' ')), qPrintable(d.created));
    }

    void createFromSource()
    {
        m_mgr.create(QStringLiteral("be-a"), QStringLiteral("default"));
        QVERIFY(m_mgr.getEnvironment(QStringLiteral("be-a")));
    }

    void createSnapshotAndCloneIt()
    {
        m_mgr.createSnapshot(QStringLiteral("default@snap1"));
        m_mgr.create(QStringLiteral("be-b"), QStringLiteral("default@snap1"));
        QVERIFY(m_mgr.getEnvironment(QStringLiteral("be-b")));
    }

    void listSnapshots()
    {
        const auto snaps = m_mgr.listSnapshots();
        const auto it = std::find_if(snaps.begin(), snaps.end(), [](const BeSnapshot &s) {
            return s.name == QLatin1String("default@snap1");
        });
        QVERIFY(it != snaps.end());
        QCOMPARE(it->environment(), QStringLiteral("default"));
        QVERIFY(!it->space.isEmpty());
        QVERIFY2(it->created.contains(QLatin1Char(' ')), qPrintable(it->created));
    }

    void destroySnapshot()
    {
        m_mgr.createSnapshot(QStringLiteral("default@descartavel"));
        m_mgr.destroy(QStringLiteral("default@descartavel"));
        for (const auto &s : m_mgr.listSnapshots())
            QVERIFY(s.name != QLatin1String("default@descartavel"));
    }

    void createSnapshotRequiresAt()
    {
        QVERIFY_THROWS_EXCEPTION(std::runtime_error, m_mgr.createSnapshot(QStringLiteral("default")));
    }

    void createDuplicateFails()
    {
        QVERIFY_THROWS_EXCEPTION(std::runtime_error,
                                 m_mgr.create(QStringLiteral("be-a"), QStringLiteral("default")));
    }

    void rename()
    {
        m_mgr.rename(QStringLiteral("be-a"), QStringLiteral("be-renamed"));
        QVERIFY(!m_mgr.getEnvironment(QStringLiteral("be-a")));
        QVERIFY(m_mgr.getEnvironment(QStringLiteral("be-renamed")));
    }

    void activate()
    {
        m_mgr.activate(QStringLiteral("be-renamed"));
        QVERIFY(be(QStringLiteral("be-renamed")).isActiveReboot());
        QVERIFY(!be(QStringLiteral("default")).isActiveReboot());
    }

    void activateTemporary()
    {
        m_mgr.activate(QStringLiteral("be-b"), true);
        const BootEnvironment b = be(QStringLiteral("be-b"));
        QVERIFY2(b.isTemporary(), qPrintable(b.active));
    }

    void mountAndUnmount()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        m_mgr.mount(QStringLiteral("be-b"), dir.path());
        QVERIFY(be(QStringLiteral("be-b")).isMounted());
        m_mgr.unmount(QStringLiteral("be-b"));
        QVERIFY(!be(QStringLiteral("be-b")).isMounted());
    }

    void destroyManyReportsEachResult()
    {
        m_mgr.activate(QStringLiteral("default"));
        const DestroyManyResult r = m_mgr.destroyMany(
            {QStringLiteral("be-b"), QStringLiteral("be-renamed"), QStringLiteral("does-not-exist")});
        QCOMPARE(r.ok, QStringList({QStringLiteral("be-b"), QStringLiteral("be-renamed")}));
        QCOMPARE(r.errors.size(), 1);
        QVERIFY(r.errors.first().startsWith(QStringLiteral("does-not-exist: ")));
        QCOMPARE(m_mgr.listEnvironments().size(), 1);
    }
};

QTEST_GUILESS_MAIN(TestBectlManager)
#include "tst_bectlmanager.moc"
