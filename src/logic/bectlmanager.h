#ifndef BECTLMANAGER_H
#define BECTLMANAGER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <optional>

/**
 * Representa um Boot Environment do FreeBSD (bectl).
 */
struct BootEnvironment {
    QString name;
    QString active;      // N, R, T, NR, ...
    QString mountpoint;
    QString space;
    QString created;

    bool isActiveNow() const { return active.contains(QLatin1Char('N')); }
    bool isActiveReboot() const { return active.contains(QLatin1Char('R')); }
    bool isTemporary() const { return active.contains(QLatin1Char('T')); }
    bool isMounted() const { return mountpoint != QLatin1String("-") && !mountpoint.isEmpty(); }
};

/**
 * Resultado de operações em lote (destroy_many).
 */
struct DestroyManyResult {
    QStringList ok;
    QStringList errors;
};

/**
 * BectlManager – lógica de comandos bectl + elevação via qt-sudo.
 *
 * Separado da interface gráfica, exatamente como na versão Python.
 */
class BectlManager
{
public:
    explicit BectlManager(bool useQtSudo = true,
                          const QString &qtSudoPath = QStringLiteral("qt-sudo"));

    // --- Leitura ---
    QVector<BootEnvironment> listEnvironments(bool scriptFormat = true,
                                              bool showSpace = false);
    std::optional<BootEnvironment> getEnvironment(const QString &name);

    // --- Escrita (via qt-sudo) ---
    QString create(const QString &name,
                   const QString &source = QString(),
                   bool recursive = false);

    QString createSnapshot(const QString &name, bool recursive = false);

    QString activate(const QString &name, bool temporary = false);

    QString destroy(const QString &name, bool force = false, bool origin = false);

    DestroyManyResult destroyMany(const QStringList &names,
                                  bool force = true,
                                  bool origin = false,
                                  bool skipActive = true);

    QString rename(const QString &oldName, const QString &newName);

    QString mount(const QString &name, const QString &mountpoint = QString());

    QString unmount(const QString &name, bool force = false);

    bool check();

private:
    struct RunResult {
        QString stdoutData;
        QString stderrData;
        int returnCode = -1;
    };

    RunResult run(const QStringList &args, bool privileged = false, int timeoutMs = 60000);
    QString runOrRaise(const QStringList &args, bool privileged = false);

    bool m_useQtSudo;
    QString m_qtSudoPath;
    QString m_bectlPath;
};

#endif // BECTLMANAGER_H
