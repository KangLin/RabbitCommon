// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLocale>
#include <QDir>
#include <QLibraryInfo>
#include <QStandardPaths>
#include <QSslSocket>
#include <QThread>
#include <QSettings>
#include <QMimeData>
#include <QProcessEnvironment>

#if defined(Q_OS_ANDROID)
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtCore/private/qandroidextras_p.h>
#endif
#endif

#ifdef HAVE_RABBITCOMMON_GUI
    #include <QIcon>
    #include <QMainWindow>
    #include <QMessageBox>
    #include <QPushButton>
    #include <QApplication>
    #include <QDesktopServices>
    #include <QClipboard>
    #include "Style.h"
    #ifdef HAVE_ABOUT
        #include "Information.h"
    #endif
#else
    #include <QCoreApplication>
#endif

#include "RabbitCommonTools.h"
#include "RabbitCommonDir.h"
#ifdef HAVE_ADMINAUTHORISER
#include "AdminAuthoriser/adminauthoriser.h"
#endif
#include "RabbitCommonRegister.h"
#include "Log/Log.h"

#if defined(HAVE_OPENSSL)
    #include "openssl/opensslv.h"
    #include "openssl/crypto.h"
    #include "openssl/ssl.h"
#endif

#if defined(Q_OS_WIN)
    #include <windows.h>
    #include <tchar.h>
//    #include <stdio.h>
    //#include "lm.h"
    //#pragma comment(lib,"netapi32.lib")
#else
    #include <pwd.h>
    #include <unistd.h>
    #include <signal.h>
#endif

#if defined(Q_OS_WIN)
    #include "CoreDump/MiniDumper.h"
#endif
#include "CoreDump/StackTrace.h"

#ifdef HAVE_RABBITCOMMON_GUI
    #include "Log/DockDebugLog.h"
    #include "Log/DlgFilter.h"
    extern CDockDebugLog* g_pDcokDebugLog;
#endif

#ifdef HAVE_CMARK
#include "cmark.h"
#endif
#ifdef HAVE_CMARK_GFM
#include "cmark-gfm.h"
#include "cmark-gfm-core-extensions.h"
#endif

inline void g_RabbitCommon_InitResource()
{
    Q_INIT_RESOURCE(ResourceRabbitCommon);

#ifdef BUILD_QUIWidget
    Q_INIT_RESOURCE(QUIWidget);
#endif
#if DEBUG
    // Must set cmake parameters: -DCMAKE_BUILD_TYPE=Debug
    //Q_INIT_RESOURCE(translations_RabbitCommon);
#ifdef BUILD_QUIWidget
    Q_INIT_RESOURCE(QUIWidgetQss);
#endif
#endif
}

inline void g_RabbitCommon_CleanResource()
{
    Q_CLEANUP_RESOURCE(ResourceRabbitCommon);
#ifdef BUILD_QUIWidget
    Q_CLEANUP_RESOURCE(QUIWidget);
#endif
#if DEBUG
    //Q_CLEANUP_RESOURCE(translations_RabbitCommon);
#ifdef BUILD_QUIWidget
    Q_CLEANUP_RESOURCE(QUIWidgetQss);
#endif
#endif
}

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Tools")
static Q_LOGGING_CATEGORY(logTranslation, "RabbitCommon.Tools.Translation")

static CCallTrace* g_pCallStack = nullptr;

CTools::CTools() : QObject()
    , m_Initialized(false)
#if defined(Q_OS_ANDROID)
    , m_bShowMaxWindow(true)
#else
    , m_bShowMaxWindow(false)
#endif
{
#ifdef HAVE_CMARK_GFM
    cmark_gfm_core_extensions_ensure_registered();
//     // free extensions at application exit (cmark-gfm is not able to register/unregister more than once)
//     std::atexit(cmark_release_plugins);
#endif
}

CTools::~CTools()
{
    qDebug(log) << "CTools::~CTools()";
#if HAVE_RABBITCOMMON_GUI
    if(g_pDcokDebugLog)
        delete g_pDcokDebugLog;
#endif
}

CTools* CTools::Instance()
{
    static CTools* pTools = nullptr;
    if(nullptr == pTools)
        pTools = new CTools();
    return pTools;
}

static QString g_szLanguage;
int CTools::SetLanguage(const QString szLanguage)
{
    g_szLanguage = szLanguage;
    return 0;
}

QString CTools::GetLanguage()
{
    QString szLANG = QProcessEnvironment::systemEnvironment().value("LANG");
    if(szLANG.isEmpty()) {
        if(g_szLanguage.isEmpty())
            return QLocale::system().name();
    } else {
        // Separate zh_CN from zh_CN.utf-8
        int underscoreIndex = szLANG.indexOf('.');
        if (underscoreIndex != -1) {
            szLANG.truncate(underscoreIndex);
        }
        return szLANG;
    }
    return g_szLanguage;
}

QString CTools::Version()
{
    QString szReturn;
    QString szVersion(RabbitCommon_VERSION);
    QString szRevision(RabbitCommon_REVISION);
    if(szRevision.isEmpty()) {
        szReturn = tr("Version: ") + szVersion;
    } else {
        szReturn = tr("Version: ") + szVersion + tr(" (From revision: "); 
#if (defined(HAVE_CMARK) || defined(HAVE_CMARK_GFM))
        szReturn += "[" + szRevision + "](http://github.com/KangLin/RabbitCommon/tree/" + szRevision + ")";
#else
        szReturn += szRevision;
#endif
        szReturn += ") ";
    }
    return szReturn;
}

QString CTools::Information()
{
    QString szInfo;
    szInfo = tr("- Functions:") + "\n";
    szInfo += tr("  - Locale: ") + GetLanguage() + "\n";
#if defined(HAVE_RABBITCOMMON_GUI)
    szInfo += tr("  - Have GUI") + "\n";
#if defined(HAVE_ABOUT)
    szInfo += tr("    - Have About dialog") + "\n";
#ifdef HAVE_CMARK_GFM
    szInfo += tr("    - Use cmark-gfm") + "\n";
#elif HAVE_CMARK
    szInfo += tr("      - Use cmark") + "\n";
#endif
#endif
#if defined(HAVE_UPDATE)
    szInfo += tr("    - Have Update") + "\n";
#endif
    szInfo += tr("    - Custom title bar for QWidget") + "\n";
    szInfo += tr("    - Dock Folder browser") + "\n";
    szInfo += tr("    - Recent menu") + "\n";
    szInfo += tr("    - Style") + "\n";
    szInfo += tr("      - Icon theme: ") + QIcon::themeName() + "\n";
    #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    szInfo += tr("      - Fall back icon theme: ") + QIcon::fallbackThemeName() + "\n";
    #endif
#endif // #if defined(HAVE_RABBITCOMMON_GUI)
    szInfo += tr("  - Log") + "\n";
    szInfo += tr("    - Core dump") + "\n";
    szInfo += tr("    - Log file: ") + CLog::Instance()->GetLogFile() + "\n";
#if defined(HAVE_OPENSSL)
    szInfo += tr("  - Have encrypt(OPENSSL)") + "\n";
#endif
    szInfo += "  - " + tr("Thread pool") + "\n";
#if defined(HAVE_CURL)
    szInfo += "  - " + tr("File transfer") + "\n";
    szInfo += "    - " + tr("libCurl") + " ";
    curl_version_info_data *version_info = curl_version_info(CURLVERSION_NOW);
    szInfo += tr("Version:") + " " + QString(version_info->version) + "\n";
    szInfo += "    - " + tr("Supported protocols:") + " ";
    if (version_info->protocols) {
        for (int i = 0; version_info->protocols[i]; i++) {
            szInfo += " ";
            szInfo += version_info->protocols[i];
        }
    }
    szInfo += "\n";
    #if CURL_VERSION_SSL
        szInfo += "    - " + QString("SSL supported:") + " " + QString(version_info->features & CURL_VERSION_SSL ? "Yes" : "No") + "\n";
    #endif
    #if CURL_VERSION_LIBSSH2
        szInfo += "    - " + QString("LIBSSH2 supported:") +  " " + QString(version_info->features & CURL_VERSION_LIBSSH2 ? "Yes" : "No") + "\n";
    #endif
#endif
#if defined(BUILD_QUIWidget)
    szInfo += tr("  - Have QUIWidget") + "\n";
#endif
    szInfo += tr("  - Application paths and files: ") + "\n";
    szInfo += tr("    - Installation root path: ") + RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot() + "\n";
    szInfo += tr("    - Application path: ") + RabbitCommon::CDir::Instance()->GetDirApplication() + "\n";
    szInfo += tr("    - Configure path: ") + RabbitCommon::CDir::Instance()->GetDirConfig() + "\n";
    szInfo += tr("    - Configure file: ") + RabbitCommon::CDir::Instance()->GetFileApplicationConfigure() + "\n";
    szInfo += tr("    - Translations path: ") + RabbitCommon::CDir::Instance()->GetDirTranslations() + "\n";
    szInfo += tr("    - Log path: ") + RabbitCommon::CDir::Instance()->GetDirLog() + "\n";
    szInfo += tr("    - Data path: ") + RabbitCommon::CDir::Instance()->GetDirData() + "\n";
    szInfo += tr("    - Icons path: ") + RabbitCommon::CDir::Instance()->GetDirIcons() + "\n";
    szInfo += tr("    - Database path: ") + RabbitCommon::CDir::Instance()->GetDirDatabase() + "\n";
    szInfo += tr("    - Database file: ") + RabbitCommon::CDir::Instance()->GetDirDatabaseFile() + "\n";
    szInfo += tr("    - Plugins path: ") + RabbitCommon::CDir::Instance()->GetDirPlugins() + "\n";
    szInfo += tr("  - User folders and files: ") + "\n";
    szInfo += tr("    - Documents path: ") + RabbitCommon::CDir::Instance()->GetDirUserDocument() + "\n";
    szInfo += tr("    - Configure path: ") + RabbitCommon::CDir::Instance()->GetDirUserConfig() + "\n";
    szInfo += tr("    - Configure file: ") + RabbitCommon::CDir::Instance()->GetFileUserConfigure() + "\n";
    szInfo += tr("    - Data path: ") + RabbitCommon::CDir::Instance()->GetDirUserData() + "\n";
    szInfo += tr("    - Image path: ") + RabbitCommon::CDir::Instance()->GetDirUserImage() + "\n";
    szInfo += tr("    - Database path: ") + RabbitCommon::CDir::Instance()->GetDirUserDatabase() + "\n";
    szInfo += tr("    - Database file: ") + RabbitCommon::CDir::Instance()->GetDirUserDatabaseFile() + "\n";

    szInfo += tr("- Dependent libraries:") + "\n";
    szInfo += tr("  - OpenSSL:") + "\n";
#if defined(HAVE_OPENSSL)
    szInfo += "    - " + tr("Build Version: ") + OPENSSL_VERSION_TEXT + "\n";
    szInfo += "    - " + tr("Runtime Version: ") + OpenSSL_version(OPENSSL_VERSION) + "\n";
#endif
    if(QSslSocket::supportsSsl())
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szInfo += "    - Qt " + tr("Build Version: ") + QSslSocket::sslLibraryBuildVersionString() + "\n";
#endif
        szInfo += "    - Qt " + tr("Installed Version: ") + QSslSocket::sslLibraryVersionString() + "\n";
    } else {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szInfo += "    - Qt " + tr("Build Version: ") + QSslSocket::sslLibraryBuildVersionString() + "\n";
#endif
        szInfo += "    - Qt " + tr("Don't install OPENSSL dynamic library. Please install it") + "\n";
    }
#if HAVE_StackWalker
    szInfo += tr("  - StackWalker") + "\n";
#endif
#ifdef HAVE_CMARK_GFM
    szInfo += tr("  - cmark-gfm") + "\n";
#elif HAVE_CMARK
    szInfo += tr("  - cmark") + "\n";
#endif

#if defined(HAVE_CURL)
    szInfo += "  - " + tr("libCurl") + " ";
    version_info = curl_version_info(CURLVERSION_NOW);
    szInfo += tr("Version:") + " " + QString(version_info->version) + "\n";
    szInfo += "  - " + tr("Supported protocols:") + " ";
    if (version_info->protocols) {
        for (int i = 0; version_info->protocols[i]; i++) {
            szInfo += " ";
            szInfo += version_info->protocols[i];
        }
    }
    szInfo += "\n";
    #if CURL_VERSION_SSL
        szInfo += "  - " + QString("SSL supported:") + " " + QString(version_info->features & CURL_VERSION_SSL ? "Yes" : "No") + "\n";
    #endif
    #if CURL_VERSION_LIBSSH2
        szInfo += "  - " + QString("LIBSSH2 supported:") +  " " + QString(version_info->features & CURL_VERSION_LIBSSH2 ? "Yes" : "No") + "\n";
    #endif
#endif

    return szInfo;
}

int CTools::AndroidRequestPermission(const QString &permission)
{
#if defined (Q_OS_ANDROID)
    #if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    // get android storage permission
    // 该接口也可以直接使用字符串，可自行跳转到头文件自行查看
    
    if (QtAndroidPrivate::checkPermission(permission).result()
        == QtAndroidPrivate::Denied)
    {
        QtAndroidPrivate::requestPermission(permission).waitForFinished();
    }
    qInfo(log) << "android permission" << permission << ";status:"
                  << QtAndroidPrivate::checkPermission(permission).result();
    #endif
#endif

    return 0;
}

int CTools::AndroidRequestPermission(const QStringList &permissions)
{
    foreach (auto p, permissions) {
        AndroidRequestPermission(p);
    }
    return 0;
}

void CTools::Init(int argc, char *argv[], QString szApplicationName)
{
    if(1 > argc || !argv) {
        qCritical(log) << "The parameters is error";
        return;
    }

    QString szName;
    QString szPath;
    QFileInfo fi(argv[0]);
    if(szApplicationName.isEmpty())
        szName = fi.baseName();
    else
        szName = szApplicationName;
    szPath = fi.absolutePath();
    Init(szName, szPath, szPath + QDir::separator() + "..");
}

void CTools::Init(QString szApplicationName,
                  QString szApplicationDirPath,
                  QString szApplicationInstallRoot,
                  const QString szLanguage)
{
    if(m_Initialized) {
        qWarning(log) << "CTools is already initialized";
        Q_ASSERT(false);
        return;
    }
#if defined(HAVE_WebEngineWidgets)
    // 修复 qtwebengine 沙箱权限问题
    if(!qEnvironmentVariable("SNAP").isEmpty()) {
        qputenv("QTWEBENGINE_DISABLE_SANDBOX", "1");
    }
#endif
    if(QCoreApplication::applicationName().isEmpty())
    {
        QCoreApplication::setApplicationName(szApplicationName);
        CDir::Instance()->SetDirUserDocument();
    }
    if(QCoreApplication::applicationDirPath().isEmpty()
        && CDir::Instance()->GetDirApplication().isEmpty())
        CDir::Instance()->SetDirApplication(szApplicationDirPath);
    if(CDir::Instance()->GetDirApplicationInstallRoot().isEmpty())
        CDir::Instance()->SetDirApplicationInstallRoot(szApplicationInstallRoot);
    if(QCoreApplication::applicationName().isEmpty()
        || CDir::Instance()->GetDirApplication().isEmpty()
        || CDir::Instance()->GetDirApplicationInstallRoot().isEmpty())
    {
        qCritical(log)
            << "CTools::Instance()->Init() is called after QApplication a(argc, argv);"
               "Or call this function with the parameters"
               "szApplicationName, "
               "szApplicationDirPath, "
               "and szApplicationInstallRoot";
        Q_ASSERT(false);
        return;
    }

    QStringList permissions;
    permissions << "android.permission.WRITE_EXTERNAL_STORAGE"
                << "android.permission.INTERNET"
                << "android.permission.ACCESS_NETWORK_STATE"
                << "android.permission.CHANGE_WIFI_STATE"
                << "android.permission.ACCESS_WIFI_STATE"
                << "android.permission.ACCESS_NETWORK_STATE"
                << "android.permission.CHANGE_NETWORK_STATE";
    AndroidRequestPermission(permissions);
    EnableCoreDump();
    RabbitCommon::CLog::Instance();
    SetLanguage(szLanguage);
    qInfo(logTranslation) << "Language:" << szLanguage;
    InitResource();
    InstallTranslator("RabbitCommon", TranslationType::Library, szLanguage);
    //Init qt translation
    QString szQtTranslationPath;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    szQtTranslationPath = QLibraryInfo::location(QLibraryInfo::TranslationsPath);
#else
    szQtTranslationPath = QLibraryInfo::path(QLibraryInfo::TranslationsPath);
#endif
    QStringList qtTranslations;
    qtTranslations << "qt" << "qtbase" << "qtmultimedia"
                   << "qtlocation";
#if HAVE_WebEngineWidgets
    qtTranslations << "qtwebengine";
#endif
    foreach(auto f, qtTranslations) {
        QString szFile = szQtTranslationPath + QDir::separator()
                 + f + "_" + szLanguage + ".qm";
        QFile file(szFile);
        if(file.exists())
            InstallTranslatorFile(szFile);
        else
            qWarning(logTranslation) << "The file doesn't exists: " << szFile;
    }

#ifdef HAVE_RABBITCOMMON_GUI
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    m_bShowMaxWindow = set.value("Tools/Window/ShowMax", m_bShowMaxWindow).toBool();

    CStyle::Instance()->LoadStyle();
#ifdef HAVE_ABOUT
    CInformation info("RabbitCommon, Qt and System information:", "");
#endif
#else
    qDebug(log) << "RabbitCommon:" << "\n" << Information();
#endif //HAVE_RABBITCOMMON_GUI
}

void CTools::Clean()
{
    foreach(auto t, m_Translator) {
        QCoreApplication::removeTranslator(t.data());
    }
    m_Translator.clear();
    CleanResource();
    delete RabbitCommon::CLog::Instance();
}

QSharedPointer<QTranslator> CTools::InstallTranslatorFile(const QString szFile)
{
    QSharedPointer<QTranslator> translator
        = QSharedPointer<QTranslator>(new QTranslator());
    if(!translator) {
        qCritical(logTranslation) << "new QTranslator fail";
        return translator;
    }
    bool bRet = translator->load(szFile);
    if(bRet)
    {
        bRet = QCoreApplication::installTranslator(translator.data());
        if(bRet)
        {
            m_Translator.push_back(translator);
            qDebug(logTranslation) << "Install translator:" << szFile;
            return translator;
        }
        else {
            qCritical(logTranslation) << "Install translator fail:" << szFile;
        }
    } else
        qCritical(logTranslation) << "Load translator file fail:" << szFile;
    return QSharedPointer<QTranslator>();
}

QSharedPointer<QTranslator> CTools::InstallTranslator(
    const QString szName,
    TranslationType type,
    const QString szPluginDir,
    const QString szLanguage)
{
    QString szTranslationName = szName;

    QString szSuffix;
    QString szPath;
    szSuffix = QDir::separator() + szTranslationName + "_" + szLanguage + ".qm";
    switch(type) {
    case TranslationType::Application:
        szPath = CDir::Instance()->GetDirTranslations();
        if(szTranslationName.isEmpty()) {
            szTranslationName = QCoreApplication::applicationName();
            szSuffix = QDir::separator() + szTranslationName + "_" + szLanguage + ".qm";
        }
        break;
    case TranslationType::Library: {
        szPath = CDir::Instance()->GetDirTranslations();
        if(szTranslationName.isEmpty()) {
            qCritical(logTranslation) << "Please set translation name";
            Q_ASSERT(false);
        }
#if defined(Q_OS_LINUX)
        QFile file(szPath + szSuffix);
        if(!file.exists()) {
            szPath = CDir::Instance()->GetDirTranslations("/usr/share");
            file.setFileName(szPath + szSuffix);
            if(!file.exists()) {
                szPath = CDir::Instance()->GetDirTranslations(
                    "/usr/local/share");
            }
        }
#endif
        break;
    }
    case TranslationType::Plugin:
        szPath = CDir::Instance()->GetDirPluginsTranslation(szPluginDir);
        if(szTranslationName.isEmpty()) {
            qCritical(logTranslation) << "Please set translation name";
            Q_ASSERT(false);
        }
        break;
    }

    QString szFile = szPath + szSuffix;
    QFile file(szFile);
    if(!file.exists())
    {
        qCritical(logTranslation) << "File isn't exit:" << szFile;
        return QSharedPointer<QTranslator>();
    }

    return InstallTranslatorFile(szFile);
}

int CTools::RemoveTranslator(QSharedPointer<QTranslator> translator)
{
    foreach(auto t, m_Translator) {
        if(t == translator) {
            QCoreApplication::removeTranslator(t.data());
            m_Translator.removeAll(t);
            return 0;
        }
    }
    return -1;
}

void CTools::InitResource()
{
    g_RabbitCommon_InitResource();
}

void CTools::CleanResource()
{
    g_RabbitCommon_CleanResource();
}

#if defined(Q_OS_UNIX)
void SigHandler(int sig)
{
    switch(sig)
    {
    case SIGSEGV: // 试图访问未分配给自己的内存, 或试图往没有写权限的内存地址写数据.
    case SIGBUS:  // 非法地址, 包括内存地址对齐(alignment)出错。比如访问一个四个字长的整数, 但其地址不是4的倍数。它与SIGSEGV的区别在于后者是由于对合法存储地址的非法访问触发的(如访问不属于自己存储空间或只读存储空间)。
    case SIGILL:  // illeage，非法的。执行了非法指令， 通常是因为可执行文件本身出现错误, 或者试图执行数据段. 堆栈溢出也有可能产生这个信号。
        qCritical(log) << "Receive exception signal:" << sig << "\n"
                       << g_pCallStack->GetStack(3).toStdString().c_str();
        break;
    default:
        qDebug(log) << "Receive signal:" << sig;
        return;
    };
    exit(sig);
}
#endif

bool CTools::EnableCoreDump(bool bPrompt)
{
    Q_UNUSED(bPrompt);
    g_pCallStack = new CCallTrace();

#ifdef Q_OS_WIN
    //static RabbitCommon::CMiniDumper dumper(bPrompt);
    RabbitCommon::EnableMiniDumper();
#else
    //SIGSEGV: 试图访问未分配给自己的内存, 或试图往没有写权限的内存地址写数据.
    signal(SIGSEGV, SigHandler);
    // 非法地址, 包括内存地址对齐(alignment)出错。比如访问一个四个字长的整数, 但其地址不是4的倍数。它与SIGSEGV的区别在于后者是由于对合法存储地址的非法访问触发的(如访问不属于自己存储空间或只读存储空间)。
    signal(SIGBUS, SigHandler);
    // illeage，非法的。执行了非法指令， 通常是因为可执行文件本身出现错误, 或者试图执行数据段. 堆栈溢出也有可能产生这个信号。
    signal(SIGILL, SigHandler);
#endif
    return true;
}

bool CTools::HasAdministratorPrivilege()
{
#ifdef HAVE_ADMINAUTHORISER
    return CAdminAuthoriser::Instance()->hasAdminRights();
#else
    return false;
#endif
}

bool CTools::ExecuteWithAdministratorPrivilege(const QString &program,
                                               const QStringList &arguments,
                                               bool bDetached)
{
#ifdef HAVE_ADMINAUTHORISER
    CAdminAuthoriser::Instance()->SetDetached(bDetached);
    return CAdminAuthoriser::Instance()->execute(program, arguments);
#else
    return false;
#endif
}

bool CTools::executeByRoot(const QString &program, const QStringList &arguments)
{
    return ExecuteWithAdministratorPrivilege(program, arguments);
}

bool CTools::StartWithAdministratorPrivilege(bool bQuitOld)
{
    bool bRet = true;
    if(!HasAdministratorPrivilege()) {
        auto para = QApplication::arguments();
        QString szApp = QApplication::applicationFilePath();
        QString szAppImage;
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0) && defined(Q_OS_WIN)
        szAppImage = qEnvironmentVariable("APPIMAGE");
#else
        szAppImage = QString::fromLocal8Bit(qgetenv("APPIMAGE"));
#endif
        qDebug(log) << "App:" << szApp << szAppImage;
        if(!szAppImage.isEmpty())
            szApp = szAppImage;
        if(!para.isEmpty())
            para.removeFirst();
        if(para.isEmpty())
            bRet = ExecuteWithAdministratorPrivilege(szApp);
        else
            bRet = ExecuteWithAdministratorPrivilege(szApp, para);
        if(bRet) {
            if(bQuitOld) {
#ifdef HAVE_RABBITCOMMON_GUI
                auto pMainWindow = GetMainWindow();
                if(pMainWindow) {
                    //pMainWindow->close(); // Note that method is not thread-safe
                    QMetaObject::invokeMethod(pMainWindow, "close", Qt::QueuedConnection);
                } else
#endif
                    //QCoreApplication::quit(); // Note that method is not thread-safe
                    if(qApp)
                        QMetaObject::invokeMethod(qApp, "quit", Qt::QueuedConnection);
            }
        }
        else
            qCritical(log) << "Start by root fail:" << QCoreApplication::applicationFilePath()
                           << para;
    }
    return bRet;
}

int CTools::InstallStartRun(const QString &szName, const QString &szPath, bool bAllUser)
{
    Q_UNUSED(szName)
    Q_UNUSED(szPath)
    Q_UNUSED(bAllUser)
    
    QString appPath;
    appPath = QCoreApplication::applicationFilePath();

    if(!szPath.isEmpty())
        appPath = szPath;
    if(appPath.isEmpty())
    {
        qCCritical(log) << "szPath is empty";
        return -1;
    }
    if(bAllUser)
        return RabbitCommon::CRegister::InstallStartRun();
    return RabbitCommon::CRegister::InstallStartRunCurrentUser();
    //See: debian/postinst and Install/install.sh
    //Ubuntu use gnome-session-properties
    // - Current user: ~/.config/autostart
    // - All user: /etc/xdg/autostart/
    //https://blog.csdn.net/DinnerHowe/article/details/79025282
    QString szLink = CRegister::GetDesktopFileLink(szName, bAllUser);
    if(QFile::exists(szLink))
        if(RemoveStartRun(szName, bAllUser))
        {
            qCCritical(log) << "RemoveStartRun" << szName << "fail";
            return -1;
        }

    appPath = CRegister::GetDesktopFileName(szPath, szName);
    QFile f(appPath);
    if(!f.exists())
    {
        qCCritical(log) << "The desktop file doesn't exist: " << appPath;
        return -2;
    }
    bool ret = f.link(szLink);
    if(!ret)
    {
        QString szCmd = "ln -s " + appPath + " " + szLink;
        if(!ExecuteWithAdministratorPrivilege(szCmd))
            qCritical(log) << "CTools::InstallStartRun: file link"
                              << f.fileName() << " to " << szLink << f.error();
        return -1;
    }
    return 0;

}

int CTools::RemoveStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);
    QString appName = QCoreApplication::applicationName();
    if(!szName.isEmpty())
        appName = szName;
    if(bAllUser)
        return RabbitCommon::CRegister::RemoveStartRun();
    return RabbitCommon::CRegister::RemoveStartRunCurrentUser();

    QString szLink = CRegister::GetDesktopFileLink(szName, bAllUser);
    if(!QFile::exists(szLink)) return 0;

    QDir d;
    if(d.remove(szLink)) return 0;
    
    QString szCmd = "rm " + szLink;
    if(ExecuteWithAdministratorPrivilege(szCmd)) return 0;
    qCritical(log) << "execute" << szCmd << "fail";
    return -1;

}

bool CTools::IsStartRun(const QString &szName, bool bAllUser)
{
    Q_UNUSED(szName);

    if(bAllUser)
        return RabbitCommon::CRegister::IsStartRun();
    return RabbitCommon::CRegister::IsStartRunCurrentUser();

    QString szLink = CRegister::GetDesktopFileLink(szName, bAllUser);
    if(QFile::exists(szLink))
        return true;
    return false;
}

int CTools::GenerateDesktopFile(const QString &szPath,
                                const QString &szName)
{
    int nRet = 0;

    QString szFile = CRegister::GetDesktopFileName(szPath, szName);

    QString szContent;
    szContent = "[Desktop Entry]\n";
    szContent += "Name=" + QCoreApplication::applicationName() + "\n";
    szContent += "Comment=" + QCoreApplication::applicationName() + "\n";
#ifdef HAVE_RABBITCOMMON_GUI
    szContent += "Name[" + QLocale::system().name() + "]=" + QApplication::applicationDisplayName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + QApplication::applicationDisplayName() + "\n";
#else
    szContent += "Name[" + QLocale::system().name() + "]=" + QCoreApplication::applicationName() + "\n";
    szContent += "Comment[" + QLocale::system().name() + "]=" + QCoreApplication::applicationName() + "\n";
#endif
    szContent += "Icon=" + QCoreApplication::applicationName() + "\n";
    szContent += "Exec=" + QCoreApplication::applicationFilePath() + "\n";
    szContent += "Categories=Application;Development;\n";
    szContent += "Terminal=false\n";
    szContent += "StartupNotify=true\n";

    QFile f(szFile);
    if(!f.open(QFile::WriteOnly))
    {
        return f.error();
    }
    f.write(szContent.toStdString().c_str());
    f.close();
    return nRet;
}

QString CTools::GetCurrentUser()
{
#ifdef UNIX
    struct passwd *pwd;
    pwd = getpwuid(getuid());
    return pwd->pw_name;
#elif WINDOWS
    CHAR szUserName[MAX_PATH] = {0};
	DWORD dwSize=MAX_PATH;
	::GetUserNameA(szUserName, &dwSize);
    return szUserName;
    //return QString::fromWCharArray(szUserName, dwSize);
#else
    return QDir::home().dirName();
#endif
}

QString CTools::GetHostName()
{
    char buf[255];
    gethostname(buf, sizeof(buf));
    return buf;
}

QString CTools::MarkDownToHtml(const QString &szText)
{
    QString szHtml = szText;
#if defined(HAVE_CMARK_GFM) || defined(HAVE_CMARK)
    char* pHtml = cmark_markdown_to_html(szText.toStdString().c_str(),
                                         szText.toStdString().length(),
                                         CMARK_OPT_DEFAULT);
    if(pHtml)
    {
        szHtml = pHtml;
        free(pHtml);
    } else {
        qCritical(log) << "cmark_markdown_to_html fail";
    }
    return szHtml;
#endif
/*
#ifdef HAVE_CMARK_GFM

    // TODO make this method which takes input and provides output: cmark_to_html()
    cmark_mem* mem = cmark_get_default_mem_allocator();
    // TODO control which extensions to use in MindForger config
    cmark_llist* syntax_extensions = cmark_list_syntax_extensions(mem);
    // TODO parse options
    cmark_parser* parser = cmark_parser_new(CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE);
    for (cmark_llist* tmp = syntax_extensions; tmp; tmp = tmp->next) {
        cmark_parser_attach_syntax_extension(parser, (cmark_syntax_extension*)tmp->data);
    }
    cmark_parser_feed(parser, szText.toStdString().c_str(), szText.toStdString().length());

    //cmark_node* doc = cmark_parse_document (markdown->c_str(), markdown->size(), CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE);
    cmark_node* doc = cmark_parser_finish(parser);
    if(doc) {
        char *rendered_html = cmark_render_html_with_mem(doc, CMARK_OPT_DEFAULT | CMARK_OPT_UNSAFE, parser->syntax_extensions, mem);
        if (rendered_html) {
            szHtml = rendered_html;
            free(rendered_html);
        }
        cmark_node_free(doc);
    }
    cmark_llist_free(mem, syntax_extensions);
    cmark_parser_free(parser);

#endif
*/
    return szHtml;
}

#ifdef HAVE_RABBITCOMMON_GUI
QAction* CTools::AddStyleMenu(QMenu *pMenu, QWidget *parent)
{
    return pMenu->addAction(QIcon::fromTheme("style"),
                            tr("Style"),
                            [=](){
                                CFrmStyle* s = new CFrmStyle(parent);
                                if(s)
                                    s->show();
                            });
}

void CTools::InsertStyleMenu(QMenu *pMenu, QAction *before, QWidget *parent)
{
    QAction* pAction = new QAction(QIcon::fromTheme("style"),
                                   tr("Style"), parent);
    if(!pAction)
        return;
    pAction->setStatusTip(tr("Style"));
    QObject::connect(pAction, &QAction::triggered, [=](){
        CFrmStyle* s = new CFrmStyle(parent);
        if(s)
            s->show();
    });
    pMenu->insertAction(before, pAction);
}

QMenu* CTools::GetLogMenu(QWidget *parentMainWindow)
{
    QMenu* pMenu = new QMenu(tr("Log"), parentMainWindow);
    if(!pMenu) return pMenu;
    pMenu->setStatusTip(tr("Log"));
    pMenu->setIcon(QIcon::fromTheme("folder-open"));
    QAction* pAction = pMenu->addAction(QIcon::fromTheme("emblem-system"),
                     tr("Settings"),
                     [](){
                         CDlgFilter dlg;
                         QString szInclude, szExclude;
                         CLog::Instance()->GetFilter(szInclude, szExclude);
                         dlg.SetFilter(szInclude, szExclude);
                         if(QDialog::Accepted == RC_SHOW_WINDOW(&dlg))
                         {
                             dlg.GetFilter(szInclude, szExclude);
                             CLog::Instance()->SetFilter(szInclude, szExclude);
                         }
                     });
    pAction->setStatusTip(pAction->text());
    pAction = pMenu->addAction(QIcon::fromTheme("document-open"),
                     tr("Open Log configure file"),
                     [](){RabbitCommon::OpenLogConfigureFile();});
    pAction->setStatusTip(pAction->text());
    pAction = pMenu->addAction(QIcon::fromTheme("document-open"),
                     tr("Open Log file"),
                     [](){RabbitCommon::OpenLogFile();});
    pAction->setStatusTip(pAction->text());
    pAction = pMenu->addAction(QIcon::fromTheme("folder-open"),
                     tr("Open Log folder"),
                     [](){RabbitCommon::OpenLogFolder();});
    pAction->setStatusTip(pAction->text());
    
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(parentMainWindow);
    if(pMainWindow) {
        pMenu->addSeparator();
        if(!g_pDcokDebugLog)
            g_pDcokDebugLog = new CDockDebugLog(pMainWindow);
        pMainWindow->addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea,
                                   g_pDcokDebugLog);
        // Must set ObjectName then restore it. See: saveState help document
        g_pDcokDebugLog->setObjectName("dockDebugLog");
        QAction* pDock = g_pDcokDebugLog->toggleViewAction();
        pMenu->addAction(pDock);
        pDock->setText(tr("Log dock"));
        pDock->setIcon(QIcon::fromTheme("floating"));
        pDock->setStatusTip(tr("Log dock"));
    } else {
        qWarning(log) << "CTools::GetLogMenu: Don't use dock debug log."
                         <<  "The parent suggest is MainWindow pointer";
    }
    
    pMenu->addSeparator();
    pAction = pMenu->addAction(QIcon::fromTheme("edit-copy"),
                               tr("Copy log file path to clipboard"),
                               [](){RabbitCommon::CopyLogFileToClipboard();});
    pAction->setStatusTip(pAction->text());
    pAction = pMenu->addAction(QIcon::fromTheme("edit-copy"),
                               tr("Copy log folder to clipboard"),
                               [](){RabbitCommon::CopyLogFolderToClipboard();});
    pAction->setStatusTip(pAction->text());

    return pMenu;
}

int CTools::RestoreWidget(QWidget *pWidget)
{
    int nRet = 0;
    Q_ASSERT(pWidget);
    if(!pWidget) return -1;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QByteArray geometry
            = set.value("MainWindow/Status/Geometry").toByteArray();
    if(!geometry.isEmpty()) {
        if(!pWidget->restoreGeometry(geometry))
            qWarning(log) << "Restore geometry fail";
    }
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(pWidget);
    if(pMainWindow) {
        QByteArray state = set.value("MainWindow/Status/State").toByteArray();
        if(!state.isEmpty()) {
            if(!pMainWindow->restoreState(state))
                qWarning(log) << "Restore state fail";
        }
    }
    return nRet;
}

int CTools::SaveWidget(QWidget *pWidget)
{
    int nRet = 0;
    Q_ASSERT(pWidget);
    if(!pWidget) return -1;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("MainWindow/Status/Geometry", pWidget->saveGeometry());
    QMainWindow* pMainWindow = qobject_cast<QMainWindow*>(pWidget);
    if(pMainWindow) {
        set.setValue("MainWindow/Status/State", pMainWindow->saveState());
    }
    return nRet;
}

int CTools::ShowWidget(QWidget *pWin)
{
    if(!pWin) {
        qCritical(log) << Q_FUNC_INFO << "The parameter is nullptr";
        Q_ASSERT(pWin);
        return -1;
    }

    QDialog* pDlg = qobject_cast<QDialog*>(pWin);
    if(pDlg) {
        if(m_bShowMaxWindow) {
            pDlg->showMaximized();
        }
        return pDlg->exec();
    } else {
        if(m_bShowMaxWindow) {
            pWin->showMaximized();
        } else
            pWin->show();
    }
    return 0;
}

void CTools::ShowCoreDialog(QString szTitle, QString szContent, QString szDetail, QString szCoreDumpFile)
{
    g_pCallStack->ShowCoreDialog(szTitle, szContent, szDetail, szCoreDumpFile);
}

#if defined(Q_OS_LINUX)
// 检测当前桌面环境
QString detectDesktopEnvironment() {
    QString desktop = qgetenv("XDG_CURRENT_DESKTOP");
    if (desktop.isEmpty()) {
        desktop = qgetenv("DESKTOP_SESSION");
    }
    return desktop.toLower();
}

// 通用的打开文件目录并选中文件的方法
bool openFileLocation(const QString &szFile) {
    bool bRet = false;
    QFileInfo fileInfo(szFile);
    if (!fileInfo.exists()) {
        qCritical(log) << "File does not exist:" << szFile;
        return false;
    }

    QString desktopEnv = detectDesktopEnvironment();
    qDebug(log) << "Detected desktop environment:" << desktopEnv;
    QStringList lstPara;
    QString szProgram;
    lstPara << "--select" << fileInfo.absoluteFilePath();
    if (desktopEnv.contains("gnome") || desktopEnv.contains("ubuntu") || desktopEnv.contains("unity")) {
        szProgram = "nautilus";
    } else if (desktopEnv.contains("kde") || desktopEnv.contains("plasma")) {
        szProgram = "dolphin";
    } else if (desktopEnv.contains("xfce")) {
        szProgram = "thunar";
    } else if (desktopEnv.contains("lxde") || desktopEnv.contains("lubuntu")) {
        szProgram = "pcmanfm";
    } else if(desktopEnv.contains("mate")) {
        szProgram = "caja";
    }
    qDebug(log) << "Program:" << szProgram << "Para:" << lstPara;
    if(!szProgram.isEmpty()) {
        bRet = QProcess::startDetached(szProgram, lstPara);
    } else {
        bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
    }
    // if(!bRet) {
    //     // 或者尝试使用 DBus 方法
    //     QUrl fileUrl = QUrl::fromLocalFile(fileInfo.absoluteFilePath());
    //     bRet = QProcess::startDetached(
    //         "dbus-send", QStringList()
    //             << "--session" 
    //             << "--type=method_call"
    //             << "--dest=org.freedesktop.FileManager1"
    //             << "/org/freedesktop/FileManager1"
    //             << "org.freedesktop.FileManager1.ShowItems"
    //             << QString("array:string:%1").arg(fileUrl.toString())
    //             << "string:");
    // }

    return bRet;
}
#endif

bool CTools::LocateFileWithExplorer(const QString szFile)
{
    bool bRet = true;
#ifdef Q_OS_WIN
    QString command = "explorer";
    QStringList args = {"/select,", QDir::toNativeSeparators(szFile)};
    bRet = QProcess::startDetached(command, args);
#elif defined(Q_OS_MAC)
    QStringList args;
    args << "-e" << QString("tell application \"Finder\" to reveal POSIX file \"%1\"").arg(szFile);
    args << "-e" << "tell application \"Finder\" to activate";
    bRet = QProcess::startDetached("osascript", args);
#elif defined(Q_OS_LINUX)
    bRet = openFileLocation(szFile);
#endif
    if(!bRet) {
        // Linux: 打开目录
        QFileInfo fileInfo(szFile);
        if (!fileInfo.exists()) {
            qCritical(log) << "File does not exist:" << szFile;
            return false;
        }
        bRet = QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
    }
    return bRet;
}

QMainWindow* CTools::GetMainWindow()
{
    foreach (QWidget *w, qApp->topLevelWidgets()) {
        if (QMainWindow* mainWin = qobject_cast<QMainWindow*>(w)) {
            return mainWin;
        }
    }
    return nullptr;
}
#endif // #ifdef HAVE_RABBITCOMMON_GUI

} //namespace RabbitCommon
