#include "bectlmanager.h"

#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>
#include <stdexcept>

BectlManager::BectlManager(bool useQtSudo, const QString &qtSudoPath)
    : m_useQtSudo(useQtSudo)
    , m_qtSudoPath(qtSudoPath)
{
    m_bectlPath = QStandardPaths::findExecutable(QStringLiteral("bectl"));
    if (m_bectlPath.isEmpty())
        m_bectlPath = QStringLiteral("/sbin/bectl");
}

BectlManager::RunResult BectlManager::run(const QStringList &args,
                                          bool privileged,
                                          int timeoutMs)
{
    QStringList cmd;

    if (privileged && m_useQtSudo) {
        QString sudo = QStandardPaths::findExecutable(m_qtSudoPath);
        if (sudo.isEmpty()) {
            throw std::runtime_error(
                QStringLiteral("'%1' não encontrado. Instale o pacote security/qt-sudo "
                               "(pkg install qt-sudo).")
                    .arg(m_qtSudoPath)
                    .toStdString());
        }
        cmd << sudo;
    }

    cmd << m_bectlPath;
    cmd << args;

    if (cmd.isEmpty()) {
        throw std::runtime_error("Lista de comando vazia");
    }

    QProcess proc;
    // Separar canais evita misturar stdout/stderr e facilita o Valgrind
    proc.setProcessChannelMode(QProcess::SeparateChannels);
    proc.start(cmd.first(), cmd.mid(1));

    if (!proc.waitForStarted(timeoutMs)) {
        const QString err = proc.errorString();
        throw std::runtime_error(
            QStringLiteral("Falha ao iniciar '%1': %2")
                .arg(cmd.join(QLatin1Char(' ')), err)
                .toStdString());
    }

    if (!proc.waitForFinished(timeoutMs)) {
        proc.kill();
        proc.waitForFinished(5000);
        throw std::runtime_error(
            QStringLiteral("Comando excedeu o tempo limite (%1 ms): %2")
                .arg(timeoutMs)
                .arg(cmd.join(QLatin1Char(' ')))
                .toStdString());
    }

    RunResult r;
    r.stdoutData = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
    r.stderrData = QString::fromLocal8Bit(proc.readAllStandardError()).trimmed();
    // Preferir exitCode; se o processo crashou, exitStatus indica
    if (proc.exitStatus() == QProcess::CrashExit) {
        throw std::runtime_error(
            QStringLiteral("Processo terminou com crash: %1")
                .arg(cmd.join(QLatin1Char(' ')))
                .toStdString());
    }
    r.returnCode = proc.exitCode();
    return r;
}

QString BectlManager::runOrRaise(const QStringList &args, bool privileged)
{
    auto r = run(args, privileged);
    if (r.returnCode != 0) {
        QString msg = r.stderrData.isEmpty() ? r.stdoutData : r.stderrData;
        if (msg.isEmpty())
            msg = QStringLiteral("Comando falhou com código %1").arg(r.returnCode);
        throw std::runtime_error(msg.toStdString());
    }
    return r.stdoutData;
}

// ---------------------------------------------------------------------------
// Leitura
// ---------------------------------------------------------------------------

QVector<BootEnvironment> BectlManager::listEnvironments(bool scriptFormat, bool showSpace)
{
    QStringList args{QStringLiteral("list")};
    if (scriptFormat)
        args << QStringLiteral("-H");
    if (showSpace)
        args << QStringLiteral("-D");

    QString stdoutData;
    try {
        stdoutData = runOrRaise(args, false);
    } catch (const std::exception &) {
        // tenta com privilégios
        stdoutData = runOrRaise(args, true);
    }

    QVector<BootEnvironment> result;
    const QStringList lines = stdoutData.split(QLatin1Char('\n'), Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QStringList parts = line.split(QLatin1Char('\t'));
        if (parts.size() < 5) {
            // fallback: espaços múltiplos
            parts = line.split(QRegularExpression(QStringLiteral("\\s+")), Qt::SkipEmptyParts);
            if (parts.size() < 5)
                continue;
            BootEnvironment be;
            be.name = parts[0];
            be.active = parts[1];
            be.mountpoint = parts[2];
            be.space = parts[3];
            be.created = parts.mid(4).join(QLatin1Char(' '));
            result.append(be);
        } else {
            BootEnvironment be;
            be.name = parts[0];
            be.active = parts[1];
            be.mountpoint = parts[2];
            be.space = parts[3];
            be.created = parts[4];
            result.append(be);
        }
    }
    return result;
}

std::optional<BootEnvironment> BectlManager::getEnvironment(const QString &name)
{
    for (const auto &be : listEnvironments()) {
        if (be.name == name)
            return be;
    }
    return std::nullopt;
}

// ---------------------------------------------------------------------------
// Escrita
// ---------------------------------------------------------------------------

QString BectlManager::create(const QString &name, const QString &source, bool recursive)
{
    QStringList args{QStringLiteral("create")};
    if (recursive)
        args << QStringLiteral("-r");
    if (!source.isEmpty())
        args << QStringLiteral("-e") << source;
    args << name;
    return runOrRaise(args, true);
}

QString BectlManager::createSnapshot(const QString &name, bool recursive)
{
    if (!name.contains(QLatin1Char('@')))
        throw std::runtime_error("O nome do snapshot deve conter '@' (ex: default@antes-upgrade)");

    QStringList args{QStringLiteral("create")};
    if (recursive)
        args << QStringLiteral("-r");
    args << name;
    return runOrRaise(args, true);
}

QString BectlManager::activate(const QString &name, bool temporary)
{
    QStringList args{QStringLiteral("activate")};
    if (temporary)
        args << QStringLiteral("-t");
    args << name;
    return runOrRaise(args, true);
}

QString BectlManager::destroy(const QString &name, bool force, bool origin)
{
    QStringList args{QStringLiteral("destroy")};
    if (force)
        args << QStringLiteral("-F");
    if (origin)
        args << QStringLiteral("-o");
    args << name;
    return runOrRaise(args, true);
}

DestroyManyResult BectlManager::destroyMany(const QStringList &names,
                                            bool force,
                                            bool origin,
                                            bool skipActive)
{
    DestroyManyResult result;
    QString activeName;

    if (skipActive) {
        try {
            for (const auto &be : listEnvironments()) {
                if (be.isActiveNow()) {
                    activeName = be.name;
                    break;
                }
            }
        } catch (...) {
            // ignora
        }
    }

    for (const QString &name : names) {
        if (skipActive && !activeName.isEmpty() && name == activeName) {
            result.errors << QStringLiteral("%1: não é possível destruir o BE ativo no momento").arg(name);
            continue;
        }
        try {
            destroy(name, force, origin);
            result.ok << name;
        } catch (const std::exception &e) {
            result.errors << QStringLiteral("%1: %2").arg(name, QString::fromUtf8(e.what()));
        }
    }
    return result;
}

QString BectlManager::rename(const QString &oldName, const QString &newName)
{
    return runOrRaise({QStringLiteral("rename"), oldName, newName}, true);
}

QString BectlManager::mount(const QString &name, const QString &mountpoint)
{
    QStringList args{QStringLiteral("mount"), name};
    if (!mountpoint.isEmpty())
        args << mountpoint;
    return runOrRaise(args, true);
}

QString BectlManager::unmount(const QString &name, bool force)
{
    QStringList args{QStringLiteral("umount")};
    if (force)
        args << QStringLiteral("-f");
    args << name;
    return runOrRaise(args, true);
}

bool BectlManager::check()
{
    auto r = run({QStringLiteral("check")}, false);
    return r.returnCode == 0;
}
