// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QApplication>
#include <QStyleHints>
#include <QLibraryInfo>
#include <QHostInfo>
#include <QTextBrowser>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QLoggingCategory>
#include <QMetaEnum>
#include <QSslSocket>
#include <QThread>
#include <QScreen>
#include <QStorageInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#include "Information.h"
#include "ui_Information.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.DlgAbout.Information")

CInformation::CInformation(const QString &szApp,
                           const QString &szInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInformation)
{
    ui->setupUi(this);
    ui->tabWidget->removeTab(0);

    SetContext(tr("Application"), szApp + szInfo);
    
    QString szRabbitCommon;
    szRabbitCommon += "# " + tr("RabbitCommon") + "\n";
    szRabbitCommon += "- " + RabbitCommon::CTools::Version() + "\n";
    szRabbitCommon += "- " + tr("Build Date/Time: ") + QString("%1/%2").arg(__DATE__, __TIME__) + "\n";
    szRabbitCommon += RabbitCommon::CTools::Information();
    SetContext(tr("RabbitCommon"), szRabbitCommon);

    QString szQt;
    szQt =  "# " + tr("Qt") + "\n";
    szQt += "- " + tr("Runtime version: ") + QString(qVersion()) + "\n";
    szQt += "- " + tr("Compile version: ") + QString(QT_VERSION_STR) + "\n";
    szQt += "- " + tr("Libraries:") + "\n";
    #if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
        szQt += "  - " + tr("Version: ")
            + QLibraryInfo::version().toString() + "\n";
    #endif
    szQt += "  - " + tr("Is debug build: ") + QString::number(QLibraryInfo::isDebugBuild()) + "\n";
    #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        szQt += "  - " + tr("Is shared build: ") + QString::number(QLibraryInfo::isSharedBuild()) + "\n";
    #endif
    szQt += "  - " + tr("Path: ") + "\n";
    szQt += GetLibrariesLocation();
    szQt += "  - " + tr("Search library paths:") + "\n";
    QStringList paths = QCoreApplication::libraryPaths();
    for (int i = 0; i < paths.size(); ++i) {
        szQt += "    - " + paths[i] + "\n";
    }
    szQt += "- " + tr("Locale: ") + QLocale::system().name() + "\n";
    szQt += "- " + tr("Icon: ") + "\n";
    szQt += "  - " + tr("Theme: ") + QIcon::themeName() + "\n";
    szQt += "    - " + tr("Search paths:") + "\n";
    foreach(auto iconPath, QIcon::themeSearchPaths())
    {
        szQt += "      - " + iconPath + "\n";
    }
    #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        szQt += "  - " + tr("Fallback theme: ") + QIcon::fallbackThemeName() + "\n";
        szQt += "    - " + tr("Fallback search paths:") + "\n";
        foreach(auto iconFallback, QIcon::fallbackSearchPaths())
        {
            szQt += "      - " + iconFallback + "\n";
        }
    #endif
    szQt += "## " + tr("Dependency libraries:") + "\n";
    szQt += tr("- OpenSSL:") + "\n";
    if(QSslSocket::supportsSsl())
    {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
            szQt += "  - " + tr("Build Version: ")
                + QSslSocket::sslLibraryBuildVersionString() + "\n";
        #endif
        szQt += "  - " + tr("Installed Version: ")
                + QSslSocket::sslLibraryVersionString() + "\n";
    } else {
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
            szQt += "  - " + tr("Build Version: ")
                + QSslSocket::sslLibraryBuildVersionString() + "\n";
        #endif
        szQt += "  - " + tr("Don't install OPENSSL dynamic library. Please install it") + "\n";
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
    szQt += "## " + tr("Standard paths:") + "\n";
    szQt += "- " + tr("Standard paths:") + "\n";
    QMetaEnum metaEnum = QMetaEnum::fromType<QStandardPaths::StandardLocation>();
    int nMaxType = metaEnum.keyCount();
    for(int i = 0; i < nMaxType; i++)
    {
        QStandardPaths::StandardLocation type = (QStandardPaths::StandardLocation)i;
        QStringList lstPath = QStandardPaths::standardLocations(type);
        if(!lstPath.isEmpty())
        {
            szQt += "  - " + QStandardPaths::displayName(type) + "["
                    + QString(metaEnum.valueToKey(i))
                    + "(" + QString::number(type) + ")]: ";
            if(lstPath.size() == 1)
                szQt += lstPath.at(0);
            else
                foreach (auto p, lstPath) {
                    szQt += QString("\n") + "    - " + p;
                }
        }
        szQt += "\n";
    }
    szQt += "- " + tr("Writable Location:") + "\n";
    for(int i = 0; i < nMaxType; i++)
    {
        QStandardPaths::StandardLocation type = (QStandardPaths::StandardLocation)i;
        QString szPath = QStandardPaths::writableLocation(type);
        if(!szPath.isEmpty()) {
            szQt += "  - " + QStandardPaths::displayName(type) + "["
                    + QString(metaEnum.valueToKey(i))
                    + "(" + QString::number(type) + ")]: ";
            szQt += szPath;
        }
        szQt += "\n";
    }
#endif // #if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))

    szQt += "\n";
    SetContext(tr("Qt"), szQt);

    QString szOS;
#if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)
    szOS = "## " + tr("OS") + "\n";
    szOS += "- " + tr("OS: ") + QSysInfo::prettyProductName() + "\n";
    szOS += "  - " + tr("Product type: ") + QSysInfo::productType() + "\n";
    szOS += "  - " + tr("Product version: ") + QSysInfo::productVersion() + "\n";
    szOS += "- " + tr("Kernel type: ") + QSysInfo::kernelType() + "\n";
    szOS += "- " + tr("Kernel version: ") + QSysInfo::kernelVersion() + "\n";
    #if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
        if(!QSysInfo::bootUniqueId().isEmpty())
            szOS += "- " + tr("Boot Id: ") + QSysInfo::bootUniqueId() + "\n";
    #endif
    szOS += "- " + tr("Build ABI: ") + QSysInfo::buildAbi() + "\n";
    szOS += "- " + tr("CPU: ") + "\n";
    int nCPU = QThread::idealThreadCount();
    szOS += "  - " + tr("Number:") + " " + QString::number(nCPU) + "\n";
    //szOS += "  - " + tr("Number:") + QString::number(std::thread::hardware_concurrency()) + "\n";
    szOS += "  - " + tr("Architecture: ")
            + QSysInfo::currentCpuArchitecture() + "\n";
    szOS += "  - " + tr("Build architecture: ")
            + QSysInfo::buildCpuArchitecture() + "\n";

    szOS += "- " + tr("Memory:") + "\n";
#ifdef Q_OS_WIN
    MEMORYSTATUSEX memStatus;
    memStatus.dwLength = sizeof(memStatus);
    if (GlobalMemoryStatusEx(&memStatus)) {
        szOS += "  - " + tr("Total physical memory: %1 GB").arg(memStatus.ullTotalPhys / (1024.0 * 1024 * 1024), 0, 'f', 2) + "\n";
        szOS += "  - " + tr("Available physical memory: %1 GB").arg(memStatus.ullAvailPhys / (1024.0 * 1024 * 1024), 0, 'f', 2) + "\n";
        szOS += "  - " + tr("Memory Usage: %1%").arg(memStatus.dwMemoryLoad) + "\n";
        szOS += "  - " + tr("Total virtual memory: %1 GB").arg(memStatus.ullTotalVirtual / (1024.0 * 1024 * 1024), 0, 'f', 2) + "\n";
        szOS += "  - " + tr("Available virtual memory: %1 GB").arg(memStatus.ullAvailVirtual / (1024.0 * 1024 * 1024), 0, 'f', 2) + "\n";
    }
#elif defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    QFile meminfo("/proc/meminfo");
    if (meminfo.open(QIODevice::ReadOnly)) {
        QByteArray content = meminfo.readAll();
        QString memStr = QString::fromUtf8(content);
        QStringList lines = memStr.split('\n', Qt::SkipEmptyParts);
        for (const QString &line : lines) {
            if (line.startsWith("MemTotal:")) {
                qint64 kb = line.split(' ', Qt::SkipEmptyParts)[1].toLongLong();
                szOS += "  - " + tr("Total physical memory: %1 GB").arg(kb / (1024.0 * 1024), 0, 'f', 2) + "\n";
            } else if (line.startsWith("MemAvailable:")) {
                qint64 kb = line.split(' ', Qt::SkipEmptyParts)[1].toLongLong();
                szOS += "  - " + tr("Available physical memory: %1 GB").arg(kb / (1024.0 * 1024), 0, 'f', 2) + "\n";
            }
        }
        meminfo.close();
    }
#endif

    szOS += "- " + tr("Storage:") + "\n";
    QList<QStorageInfo> storages = QStorageInfo::mountedVolumes();
    for (const QStorageInfo &storage : storages) {
        if (storage.isValid() && storage.isReady()) {
            QString rootPath = storage.rootPath();
            quint64 total = storage.bytesTotal();
            quint64 available = storage.bytesAvailable();
            quint64 used = total - available;
            double percentUsed = (total > 0) ? (used * 100.0 / total) : 0;

            szOS += "  - " +
#ifdef Q_OS_WIN
                    tr("Volume letter:")
#else
                    tr("Root:")
#endif
                    + " " + (rootPath.isEmpty() ? "/" : rootPath) + "\n";
            szOS += "    - " + tr("Total capacity: %1 GB").arg(total / (1024.0 * 1024 * 1024), 0, 'f', 2) + "\n";
            szOS += "    - " + tr("Used: %1 GB (%2%)").arg(used / (1024.0 * 1024 * 1024), 0, 'f', 2)
                            .arg(percentUsed, 0, 'f', 1) + "\n";
            szOS += "    - " + tr("Available: %1 GB").arg(available / (1024.0 * 1024 * 1024), 0, 'f', 2) + "\n";
            szOS += "    - " + tr("File system type: %1").arg(storage.fileSystemType());
            szOS.append("\n");
        }
    }

    #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        szOS += "- " + tr("Theme:") + " ";
        switch(QApplication::styleHints()->colorScheme()) {
        case Qt::ColorScheme::Dark:
            szOS += tr("Dark");
            break;
        case Qt::ColorScheme::Light:
            szOS += tr("Light");
            break;
        case Qt::ColorScheme::Unknown:
            szOS += tr("Unknown");
            break;
        }
        szOS += "\n";
    #endif // #if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)

    QString szHost;
    szHost = "## " + tr("Host") + "\n";
    #if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
        szHost += "- " + tr("Host name: ") + QSysInfo::machineHostName() + "\n";
    #endif
    szHost += "- " + tr("Domain name: ") + QHostInfo::localDomainName() + "\n";
#endif // #if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)

#if HAVE_RABBITCOMMON_GUI
    QString szScreen;
    szScreen += "## " + tr("Screen") + "\n";
    auto screens = QGuiApplication::screens();
    szScreen += "  - " + tr("Total:") + " " + QString::number(screens.size()) + "\n";
    int indexScreen = 1;
    foreach(QScreen* s, screens) {
        szScreen += "    - [" + QString::number(indexScreen++);
        if(s == QGuiApplication::primaryScreen())
            szScreen += " - " + tr("Primery");
        szScreen += "] " + tr("Name") + ": " + s->name() + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 12, 0)
        if(!s->manufacturer().isEmpty())
            szScreen += "      - " + tr("Manufacturer") + ": " + s->manufacturer() + "\n";
        if(!s->serialNumber().isEmpty())
            szScreen += "      - " + tr("Serial Number") + ": " + s->serialNumber() + "\n";
        if(!s->model().isEmpty())
            szScreen += "      - " + tr("Model") + ": " + s->model() + "\n";
#endif
        szScreen += "      - " + tr("Refresh Rate") + ": " + QString::number(s->refreshRate()) + "\n";
        szScreen += "      - " + tr("Depth") + ": " + QString::number(s->depth()) + "\n";
        szScreen += "      - " + tr("Device Pixel Ratio") + ": " + QString::number(s->devicePixelRatio()) + "\n";
        szScreen += "      - " + tr("Logical Dots Per Inch") + ": " + QString::number(s->logicalDotsPerInch()) + "\n";
        szScreen += "        - " + tr("Logical Dots Per Inch X") + ": " + QString::number(s->logicalDotsPerInchX()) + "\n";
        szScreen += "        - " + tr("Logical Dots Per Inch Y") + ": " + QString::number(s->logicalDotsPerInchY()) + "\n";
        szScreen += "      - " + tr("Physical Dots Per Inch") + ": " + QString::number(s->physicalDotsPerInch()) + "\n";
        szScreen += "        - " + tr("Physical Dots Per Inch X") + ": " + QString::number(s->physicalDotsPerInchX()) + "\n";
        szScreen += "        - " + tr("Physical Dots Per Inch Y") + ": " + QString::number(s->physicalDotsPerInchY()) + "\n";
        szScreen += "      - " + tr("Physical size") + ": " + QString::number(s->physicalSize().width()) + "×" + QString::number(s->physicalSize().height()) + "\n";
        szScreen += "      - " + tr("Size") + ": " + QString::number(s->size().width()) + "×" + QString::number(s->size().height()) + "\n";
        szScreen += "      - " + tr("Geometry") + ": (" + QString::number(s->geometry().left()) + "," + QString::number(s->geometry().top()) + "," + QString::number(s->geometry().width()) + "," + QString::number(s->geometry().height()) + ")\n";
        szScreen += "      - " + tr("Available Size") + ": " + QString::number(s->availableSize().width()) + "×" + QString::number(s->availableSize().height()) + "\n";
        szScreen += "      - " + tr("Available Geometry") + ": (" + QString::number(s->availableGeometry().left()) + "," + QString::number(s->availableGeometry().top()) + "," + QString::number(s->availableGeometry().width()) + "," + QString::number(s->availableGeometry().height()) + ")\n";
        auto siblings = s->virtualSiblings();
        if(siblings.size() > 0) {
            szScreen += "      - " + tr("Siblings") + ": " + QString::number(siblings.size()) + "\n";
        }
        szScreen += "      - " + tr("Virtual Size") + ": " + QString::number(s->virtualSize().width()) + "×" + QString::number(s->virtualSize().height()) + "\n";
        szScreen += "      - " + tr("Virtual Geometry") + ": (" + QString::number(s->virtualGeometry().left()) + "," + QString::number(s->virtualGeometry().top()) + "," + QString::number(s->virtualGeometry().width()) + "," + QString::number(s->virtualGeometry().height()) + ")\n";
        szScreen += "      - " + tr("Available Virtual Size") + ": " + QString::number(s->availableVirtualSize().width()) + "×" + QString::number(s->availableVirtualSize().height()) + "\n";
        szScreen += "      - " + tr("Available Virtual Geometry") + ":( " + QString::number(s->availableVirtualGeometry().left()) + "," + QString::number(s->availableVirtualGeometry().top()) + "," + QString::number(s->availableVirtualGeometry().width()) + "," + QString::number(s->availableVirtualGeometry().height()) + ")\n";
    }
#endif // #if HAVE_RABBITCOMMON_GUI

    QString szEnv;
    szEnv += "## " + tr("Environment") + "\n";
    auto env = QProcessEnvironment::systemEnvironment();
    foreach (auto key, env.keys()) {
        szEnv += "  - " + key + "=" + env.value(key) + "\n";
    }

    if(!szHost.isEmpty())
        SetContext(tr("Host"), szHost + szOS + szScreen + szEnv);

    qDebug(log) << (szApp + szInfo + szRabbitCommon
                    + szQt + szOS + szHost + szEnv).toStdString().c_str();
}

CInformation::~CInformation()
{
    delete ui;
}

void CInformation::SetContext(const QString& szTitle, const QString& szContext)
{
    if(szTitle.isEmpty() || szContext.isEmpty())
    {
        qCritical(log) << "Title or context is empty";
        return;
    }
    QString szHtml;
    #if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        szHtml = RabbitCommon::CTools::MarkDownToHtml(szContext);
    #endif
    if(szHtml.isEmpty())
        szHtml = szContext;
    QTextBrowser* pEdit = new QTextBrowser(ui->tabWidget);
    if(!pEdit) return;
    pEdit->setOpenExternalLinks(true);
    pEdit->setOpenLinks(true);
    pEdit->setReadOnly(true);
    pEdit->setWordWrapMode(QTextOption::NoWrap);
    #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        pEdit->setMarkdown(szContext);
    #else
        pEdit->setHtml(szHtml);
    #endif
    //把光标移动文档开始处
    QTextCursor cursor = pEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    pEdit->setTextCursor(cursor);

    pEdit->show();
    ui->tabWidget->addTab(pEdit, szTitle);
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QString CInformation::GetLibrariesLocation(QLibraryInfo::LibraryLocation path)
#else
QString CInformation::GetLibrariesLocation(QLibraryInfo::LibraryPath path)
#endif
{
    QString szQt;
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        szQt += QLibraryInfo::location(path);
    #elif QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
        szQt += QLibraryInfo::path(path);
    #else
        foreach(auto s, QLibraryInfo::paths(path))
        {
            szQt += s;
        }
    #endif
    return szQt;
}

QString CInformation::GetLibrariesLocation()
{
    QString szQt;
    szQt += "    - PrefixPath: " + GetLibrariesLocation(QLibraryInfo::PrefixPath) + "\n";
    szQt += "    - DocumentationPath: " + GetLibrariesLocation(QLibraryInfo::DocumentationPath) + "\n";
    szQt += "    - HeadersPath: " + GetLibrariesLocation(QLibraryInfo::HeadersPath) + "\n";
    szQt += "    - LibrariesPath: " + GetLibrariesLocation(QLibraryInfo::LibrariesPath) + "\n";
    szQt += "    - LibraryExecutablesPath: " + GetLibrariesLocation(QLibraryInfo::LibraryExecutablesPath) + "\n";
    szQt += "    - BinariesPath: " + GetLibrariesLocation(QLibraryInfo::BinariesPath) + "\n";
    szQt += "    - PluginsPath: " + GetLibrariesLocation(QLibraryInfo::PluginsPath) + "\n";
    #if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        szQt += "    - QmlImportsPath: " + GetLibrariesLocation(QLibraryInfo::QmlImportsPath) + "\n";
    #endif
    szQt += "    - ArchDataPath: " + GetLibrariesLocation(QLibraryInfo::ArchDataPath) + "\n";
    szQt += "    - DataPath: " + GetLibrariesLocation(QLibraryInfo::DataPath) + "\n";
    szQt += "    - TranslationsPath: " + GetLibrariesLocation(QLibraryInfo::TranslationsPath) + "\n";
    szQt += "    - ExamplesPath: " + GetLibrariesLocation(QLibraryInfo::ExamplesPath) + "\n";
    szQt += "    - TestsPath: " + GetLibrariesLocation(QLibraryInfo::TestsPath) + "\n";
    szQt += "    - SettingsPath: " + GetLibrariesLocation(QLibraryInfo::SettingsPath) + "\n";
    return szQt;
}
