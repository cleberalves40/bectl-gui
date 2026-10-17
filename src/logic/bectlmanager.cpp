#include "bectlmanager.h"
#include "i18n.h"

#include <QProcess>
#include <QStandardPaths>
#include <QRegularExpression>
#include <stdexcept>

namespace {

[[noreturn]] void fail(const QString &msg)
{
    throw std::runtime_error(msg.toStdString());
}

// Mesmas regras do libbe (be_validate_name/zfs_name_valid): letras, dígitos e
// "_-.:", sem espaços. Além disso, recusa "-" inicial: o nome vai ao bectl(8)
// como argumento, e ele o interpretaria como opção.
bool validComponent(const QString &s)
{
    static const QRegularExpression re(QStringLiteral("^[A-Za-z0-9_.:][A-Za-z0-9_.:-]*$"));
    return s.size() <= 255 && s != QLatin1String(".") && s != QLatin1String("..")
           && re.match(s).hasMatch();
}

void requireBeName(const QString &name)
{
    if (!validComponent(name))
        fail(I18n::tr1("Invalid boot environment name: %1", name));
}

// "be" ou "be@snapshot".
void requireBeOrSnapshot(const QString &name)
{
    const QStringList parts = name.split(QLatin1Char('@'));
    if (parts.size() > 2 || !validComponent(parts.first())
        || (parts.size() == 2 && !validComponent(parts.last())))
        fail(I18n::tr1(parts.size() == 1 ? "Invalid boot environment name: %1"
                                         : "Invalid snapshot name: %1",
                       name));
}

} // namespace

BectlManager::BectlManager(bool useQtSudo, const QString &qtSudoPath, const QString &bectlPath)
    : m_useQtSudo(useQtSudo)
    , m_qtSudoPath(qtSudoPath)
    , m_bectlPath(bectlPath)
{
    if (m_bectlPath.isEmpty())
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
            fail(I18n::tr1("'%1' not found. Install the security/qt-sudo package "
                           "(pkg install qt-sudo).",
                           m_qtSudoPath));
        }
        cmd << sudo;
    }

    cmd << m_bectlPath;
    cmd << args;

    QProcess proc;
    // Separar canais evita misturar stdout/stderr e facilita o Valgrind
    proc.setProcessChannelMode(QProcess::SeparateChannels);
    proc.start(cmd.first(), cmd.mid(1));

    if (!proc.waitForStarted(30000))
        fail(I18n::tr2("Failed to start '%1': %2", cmd.join(QLatin1Char(' ')), proc.errorString()));

    // Comandos privilegiados não têm limite de tempo: a espera inclui a senha
    // digitada no qt-sudo, e operações como destroy podem ser longas.
    const int limit = privileged && m_useQtSudo ? -1 : timeoutMs;
    if (!proc.waitForFinished(limit)) {
        proc.kill();
        proc.waitForFinished(5000);
        fail(I18n::tr2("Command timed out (%1 ms): %2", QString::number(timeoutMs),
                       cmd.join(QLatin1Char(' '))));
    }

    RunResult r;
    r.stdoutData = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
    r.stderrData = QString::fromLocal8Bit(proc.readAllStandardError()).trimmed();
    // Preferir exitCode; se o processo crashou, exitStatus indica
    if (proc.exitStatus() == QProcess::CrashExit)
        fail(I18n::tr1("Process crashed: %1", cmd.join(QLatin1Char(' '))));
    r.returnCode = proc.exitCode();
    return r;
}

QString BectlManager::runOrRaise(const QStringList &args, bool privileged)
{
    auto r = run(args, privileged);
    if (r.returnCode != 0) {
        QString msg = r.stderrData.isEmpty() ? r.stdoutData : r.stderrData;
        if (msg.isEmpty())
            msg = I18n::tr1("Command failed with exit code %1", QString::number(r.returnCode));
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
    requireBeName(name);
    if (!source.isEmpty())
        requireBeOrSnapshot(source);
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
        fail(I18n::tr("Snapshot name must contain '@' (e.g. default@before-upgrade)"));
    requireBeOrSnapshot(name);

    QStringList args{QStringLiteral("create")};
    if (recursive)
        args << QStringLiteral("-r");
    args << name;
    return runOrRaise(args, true);
}

QString BectlManager::activate(const QString &name, bool temporary)
{
    requireBeName(name);
    QStringList args{QStringLiteral("activate")};
    if (temporary)
        args << QStringLiteral("-t");
    args << name;
    return runOrRaise(args, true);
}

QString BectlManager::destroy(const QString &name, bool force, bool origin)
{
    requireBeOrSnapshot(name);
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
            result.errors << I18n::tr1("%1: cannot destroy the currently active BE", name);
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
    requireBeName(oldName);
    requireBeName(newName);
    return runOrRaise({QStringLiteral("rename"), oldName, newName}, true);
}

QString BectlManager::mount(const QString &name, const QString &mountpoint)
{
    requireBeName(name);
    if (!mountpoint.isEmpty() && !mountpoint.startsWith(QLatin1Char('/')))
        fail(I18n::tr1("Mount point must be an absolute path: %1", mountpoint));
    QStringList args{QStringLiteral("mount"), name};
    if (!mountpoint.isEmpty())
        args << mountpoint;
    return runOrRaise(args, true);
}

QString BectlManager::unmount(const QString &name, bool force)
{
    requireBeName(name);
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
