// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QLibraryInfo>
#include <QHostInfo>
#ifdef HAVE_WebEngineWidgets
#include <QWebEngineView>
#endif
#include <QTextEdit>
#include <QStandardPaths>
#include <QProcessEnvironment>
#include <QLoggingCategory>

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
    szQt += "- " + tr("Qt runtime version: ") + QString(qVersion()) + "\n";
    szQt += "- " + tr("Qt compile version: ") + QString(QT_VERSION_STR) + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    szQt += "- " + tr("Qt library version: ") + QLibraryInfo::version().toString() + "\n";
#endif
    szQt += "- " + tr("Locale: ") + QLocale::system().name() + "\n";
    szQt += "\n";
    szQt += tr("- OpenSSL:") + "\n";
    if(QSslSocket::supportsSsl())
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szQt += "  - " + tr("Build Version: ") + QSslSocket::sslLibraryBuildVersionString() + "\n";
#endif
        szQt += "  - " + tr("Installed Version: ") + QSslSocket::sslLibraryVersionString() + "\n";
    } else {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 3))
        szQt += "  - " + tr("Build Version: ") + QSslSocket::sslLibraryBuildVersionString() + "\n";
#endif
        szQt += "  - " + tr("Don't install OPENSSL dynamic library. Please install it") + "\n";
    }
    szQt += "- " + tr("Standard paths:") + "\n";
    int nMaxType = 20;
    for(int i = 0; i <= nMaxType; i++)
    {
        QStandardPaths::StandardLocation type = (QStandardPaths::StandardLocation)i;
        QStringList lstPath = QStandardPaths::standardLocations(type);
        if(!lstPath.isEmpty())
        {
            szQt += "  - " + QStandardPaths::displayName(type) + "[" + QString::number(type) + "]: ";
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
    for(int i = 0; i <= nMaxType; i++)
    {
        QStandardPaths::StandardLocation type = (QStandardPaths::StandardLocation)i;
        QString szPath = QStandardPaths::writableLocation(type);
        if(!szPath.isEmpty()) {
            szQt += "  - " + QStandardPaths::displayName(type) + "[" + QString::number(type) + "]: ";
            szQt += szPath;
        }
        szQt += "\n";
    }
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
    szOS += "  - " + tr("Architecture: ") + QSysInfo::currentCpuArchitecture() + "\n";
    szOS += "  - " + tr("Build architecture: ") + QSysInfo::buildCpuArchitecture() + "\n";
    
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

    qDebug(log) << (szApp + szInfo + szRabbitCommon + szQt + szOS + szHost + szEnv).toStdString().c_str();
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

#if (defined(HAVE_CMARK) || defined(HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
    QWebEngineView* pEdit = new QWebEngineView(ui->tabWidget);
    if(pEdit) {
        pEdit->setHtml(CDlgAbout::MarkDownToHtml(szContext));
        pEdit->show();
        ui->tabWidget->addTab(pEdit, szTitle);
    } else
#endif
    {
        QTextEdit* pEdit = new QTextEdit(ui->tabWidget);
        if(!pEdit) return;
        pEdit->setReadOnly(true);
        pEdit->setWordWrapMode(QTextOption::NoWrap);
        ui->tabWidget->addTab(pEdit, szTitle);
        pEdit->append(szContext);
        //把光标移动文档开始处
        QTextCursor cursor = pEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        pEdit->setTextCursor(cursor);
        pEdit->show();
    }
}
