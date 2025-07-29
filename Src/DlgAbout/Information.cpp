// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLibraryInfo>
#include <QHostInfo>
#include <QTextEdit>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QLoggingCategory>
#include <QMetaEnum>
#include <QSslSocket>
#ifdef HAVE_WebEngineWidgets
#include <QWebEngineView>
#endif

#include "Information.h"
#include "ui_Information.h"
#include "RabbitCommonTools.h"
#include "DlgAbout.h"

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
    szRabbitCommon = "\n" + tr("### RabbitCommon") + "\n";
    szRabbitCommon += "- " + RabbitCommon::CTools::Version() + "\n";
    szRabbitCommon += RabbitCommon::CTools::Information();
    SetContext(tr("RabbitCommon"), szRabbitCommon);
    
    QString szQt;
    szQt = tr("### Qt") + "\n";
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
    szQt += "### " + tr("Dependency libraries:") + "\n";
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
    szQt += "### " + tr("Standard paths:") + "\n";
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
#endif
    szQt += "\n";
    SetContext(tr("Qt"), szQt);
    
    QString szOS;
#if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)
    szOS = "### " + tr("OS") + "\n";
    szOS += "- " + tr("OS: ") + QSysInfo::prettyProductName() + "\n";
    szOS += "  - " + tr("product type: ") + QSysInfo::productType() + "\n";
    szOS += "  - " + tr("product version: ") + QSysInfo::productVersion() + "\n";
    szOS += "- " + tr("Kernel type: ") + QSysInfo::kernelType() + "\n";
    szOS += "- " + tr("Kernel version: ") + QSysInfo::kernelVersion() + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
    if(!QSysInfo::bootUniqueId().isEmpty())
        szOS += "- " + tr("Boot Id: ") + QSysInfo::bootUniqueId() + "\n";
#endif
    szOS += "- " + tr("Build ABI: ") + QSysInfo::buildAbi() + "\n";
    szOS += "- " + tr("CPU: ") + "\n";
    szOS += "  - " + tr("Architecture: ")
            + QSysInfo::currentCpuArchitecture() + "\n";
    szOS += "  - " + tr("Build architecture: ")
            + QSysInfo::buildCpuArchitecture() + "\n";
    
    QString szHost;
    szHost = "### " + tr("Host") + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
    szHost += "- " + tr("Host name: ") + QSysInfo::machineHostName() + "\n";
#endif
    szHost += "- " + tr("Domain name: ") + QHostInfo::localDomainName() + "\n";
#endif

    QString szEnv;
    szEnv += "### " + tr("Environment:") + "\n";
    auto env = QProcessEnvironment::systemEnvironment();
    foreach (auto key, env.keys()) {
        szEnv += "  - " + key + "=" + env.value(key) + "\n";
    }

    if(!szHost.isEmpty())
        SetContext(tr("Host"), szHost + szOS + szEnv);

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
#if defined(HAVE_WebEngineWidgets)
    QWebEngineView* pEdit = new QWebEngineView(ui->tabWidget);
    if(!pEdit) return;
    pEdit->setHtml(szHtml);
#else
    QTextEdit* pEdit = new QTextEdit(ui->tabWidget);
    if(!pEdit) return;
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
#endif
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
    szQt += "\n";
    return szQt;
}

QString CInformation::GetLibrariesLocation()
{
    QString szQt;
    szQt += "    - PrefixPath: " + GetLibrariesLocation(QLibraryInfo::PrefixPath);
    szQt += "    - DocumentationPath: " + GetLibrariesLocation(QLibraryInfo::DocumentationPath);
    szQt += "    - HeadersPath: " + GetLibrariesLocation(QLibraryInfo::HeadersPath);
    szQt += "    - LibrariesPath: " + GetLibrariesLocation(QLibraryInfo::LibrariesPath);
    szQt += "    - LibraryExecutablesPath: " + GetLibrariesLocation(QLibraryInfo::LibraryExecutablesPath);
    szQt += "    - BinariesPath: " + GetLibrariesLocation(QLibraryInfo::BinariesPath);
    szQt += "    - PluginsPath: " + GetLibrariesLocation(QLibraryInfo::PluginsPath);
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
    szQt += "    - QmlImportsPath: " + GetLibrariesLocation(QLibraryInfo::QmlImportsPath);
#endif
    szQt += "    - ArchDataPath: " + GetLibrariesLocation(QLibraryInfo::ArchDataPath);
    szQt += "    - DataPath: " + GetLibrariesLocation(QLibraryInfo::DataPath);
    szQt += "    - TranslationsPath: " + GetLibrariesLocation(QLibraryInfo::TranslationsPath);
    szQt += "    - ExamplesPath: " + GetLibrariesLocation(QLibraryInfo::ExamplesPath);
    szQt += "    - TestsPath: " + GetLibrariesLocation(QLibraryInfo::TestsPath);
    szQt += "    - SettingsPath: " + GetLibrariesLocation(QLibraryInfo::SettingsPath);
    return szQt;
}
