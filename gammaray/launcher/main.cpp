
#include <sgi/Export>
#include <launcher/launchoptions.h>
#include <launcher/launcher.h>
#include <launcher/probeabi.h>

#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLibrary>
#include <QDir>
#include <QDebug>

#define GAMMARAY_PROBE_LIBRARY_NAME "gammaray_probe" SGI_LIBRARY_SHARED_MODULE_SUFFIX
#define GAMMARAY_PROBE_FUNCTION "gammaray_probe_inject"

namespace gammaray {
    class ProbeABI {
    public:
        static bool isDebugRelevant()
        {
        #if defined(Q_OS_MACX)
            return true;
        #elif defined(_MSC_VER)
            return true;
        #else
            return false;
        #endif
        }
        static bool isDebug()
        {
        #ifdef _DEBUG
            return true;
        #else
            return false;
        #endif
        }

        static QString id()
        {
            QStringList idParts;
            idParts.push_back(QStringLiteral("qt%1_%2").arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR));

        #ifdef Q_OS_WIN
            #ifdef _MSC_VER
                idParts.push_back(QStringLiteral("MSVC"));
            #endif
        #endif

        #ifdef _MSC_VER
            #if defined(_M_AMD64) || defined(_M_X64)
                idParts.push_back(QStringLiteral("x86_64"));
            #else
                idParts.push_back(QStringLiteral("i686"));
            #endif
        #else
            #if defined(__amd64__) || defined(__x86_64__)
                idParts.push_back(QStringLiteral("x86_64"));
            #else
                idParts.push_back(QStringLiteral("x86"));
            #endif
        #endif
            return idParts.join(QStringLiteral("-")).append((isDebugRelevant() && isDebug()) ?
                    QStringLiteral(SGI_LIBRARY_POSTFIX_DEBUG) : QString());
        }
        static QString probePath()
        {
#ifdef _WIN32
            QString prefix = QLibraryInfo::location(QLibraryInfo::BinariesPath);
#else
            QString prefix = QLibraryInfo::location(QLibraryInfo::PrefixPath);
#endif
            return QDir::toNativeSeparators(prefix + QDir::separator()
                + QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR) + QDir::separator()
                + QLatin1String(GAMMARAY_PLUGIN_VERSION) + QDir::separator()
                + id() + QDir::separator()
                + QLatin1String(GAMMARAY_PROBE_LIBRARY_NAME));
        }
    };

    bool loadProbe()
    {
        QString probeDllPath;
        const QByteArray probeDllPathEnv = qgetenv("GAMMARAY_SGIINJECTOR_PROBEDLL");
        if (probeDllPathEnv.isEmpty())
            probeDllPath = ProbeABI::probePath();
        else
            probeDllPath = probeDllPathEnv;

        if (probeDllPath.isEmpty()) {
            qWarning("No probe DLL specified.");
            return false;
        }

        QByteArray probeFunc = qgetenv("GAMMARAY_SGIINJECTOR_PROBEFUNC");
        if (probeFunc.isEmpty())
            probeFunc = QByteArray(GAMMARAY_PROBE_FUNCTION);

        if (probeFunc.isEmpty()) {
            qWarning("No probe function specified.");
            return false;
        }

        QLibrary probeDll(probeDllPath);
        probeDll.setLoadHints(QLibrary::ResolveAllSymbolsHint);
        if (!probeDll.load()) {
            qWarning() << "Loading probe DLL failed:" << probeDll.errorString();
            return false;
        }

        QFunctionPointer probeFuncHandle = probeDll.resolve(probeFunc);
        if (!probeFuncHandle) {
            qWarning() << "Resolving probe function failed:" << probeDll.errorString();
            return false;
        }
        reinterpret_cast<void (*)()>(probeFuncHandle)();
        return true;
    }
} // namespace gammaray

SGI_EXPORT int run_launcher()
{
    QCoreApplication * app = QCoreApplication::instance();
    GammaRay::LaunchOptions options;
    GammaRay::Launcher launcher(options);
    options.setPid(QCoreApplication::applicationPid());
    options.setUiMode(GammaRay::LaunchOptions::InProcessUi);
    GammaRay::ProbeABI probeabi;
    probeabi.setQtVersion(QT_VERSION_MAJOR, QT_VERSION_MINOR);
#ifdef _MSC_VER
    #if defined(_M_AMD64) || defined(_M_X64)
        probeabi.setArchitecture(QStringLiteral("x86_64"));
    #else
        probeabi.setArchitecture(QStringLiteral("i686"));
    #endif
#else
    #if defined(__amd64__) || defined(__x86_64__)
        probeabi.setArchitecture(QStringLiteral("x86_64"));
    #else
        probeabi.setArchitecture(QStringLiteral("x86"));
    #endif
#endif
#ifdef _DEBUG
    probeabi.setIsDebug(true);
#else
    probeabi.setIsDebug(false);
#endif
#ifdef _MSC_VER
    probeabi.setCompiler(QStringLiteral("MSVC"));
#endif
    options.setProbeABI(probeabi);
    options.setInjectorType("style");

    //QObject::connect(&launcher, SIGNAL(finished()), &app, SLOT(quit()));
    //QObject::connect(&launcher, SIGNAL(attached()), &app, SLOT(quit()));
    if (!launcher.start())
        return launcher.exitCode();
    return 0;
}

