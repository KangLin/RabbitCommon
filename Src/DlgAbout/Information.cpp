#include "Information.h"
#include "ui_Information.h"
#include "RabbitCommonTools.h"
#include "DlgAbout.h"
#include "Log/Log.h"

#include <QLibraryInfo>
#include <QHostInfo>
#ifdef HAVE_WebEngineWidgets
#include <QWebEngineView>
#endif
#include <QTextEdit>
#include <QStandardPaths>

CInformation::CInformation(const QString &szApp, const QString &szInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInformation)
{
    ui->setupUi(this);
    ui->tabWidget->removeTab(0);

    QString szRabbitCommon, szOS, szQt, szHost;

    szRabbitCommon += "\n" + tr("### RabbitCommon") + "\n";
    szRabbitCommon += "- " + RabbitCommon::CTools::Version() + "\n";
    szRabbitCommon += RabbitCommon::CTools::Information();

    szQt += tr("### Qt") + "\n";
    szQt += "- " + tr("Qt runtime version: ") + QString(qVersion()) + "\n";
    szQt += "- " + tr("Qt compile version: ") + QString(QT_VERSION_STR) + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    szQt += "- " + tr("Qt library version: ") + QLibraryInfo::version().toString() + "\n";
#endif
    szQt += "- " + tr("Locale: ") + QLocale::system().name() + "\n";
    szQt += "\n";
    szQt += "- " + tr("Standard paths:") + "\n";
    for(int i = 0; i < 19; i++)
    {
        QStandardPaths::StandardLocation type = (QStandardPaths::StandardLocation)i;
        szQt += "  - " + QStandardPaths::displayName(type) + ": ";
        QStringList lstPath = QStandardPaths::standardLocations(type);
        if(lstPath.size() == 1)
            szQt += lstPath.at(0);
        else
            foreach (auto p, lstPath) {
                szQt += QString("\n") + "    - " + p;
            }
        szQt += "\n";
    }
    szQt += "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 4, 0)
    szOS += tr("### OS") + "\n";
    szOS += "- " + tr("OS: ") + QSysInfo::prettyProductName() + "\n";
    szOS += "- " + tr("Kernel type: ") + QSysInfo::kernelType() + "\n";
    szOS += "- " + tr("Kernel version: ") + QSysInfo::kernelVersion() + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
    if(!QSysInfo::bootUniqueId().isEmpty())
        szOS += "- " + tr("Boot Id: ") + QSysInfo::bootUniqueId() + "\n";
#endif
    szOS += "- " + tr("Build ABI: ") + QSysInfo::buildAbi() + "\n";
    szOS += "- " + tr("CPU: ") + QSysInfo::currentCpuArchitecture() + "\n";
    szOS += "- " + tr("Build CPU: ") + QSysInfo::buildCpuArchitecture() + "\n";

    szHost += tr("### Host") + "\n";
#if QT_VERSION > QT_VERSION_CHECK(5, 6, 0)
    szHost += "- " + tr("Host name: ") + QSysInfo::machineHostName() + "\n";
#endif
    szHost += "- " + tr("Domain name: ") + QHostInfo::localDomainName();
#endif
    SetContext(tr("Application"), szApp + szInfo);
    SetContext(tr("RabbitCommon"), szRabbitCommon);
    //SetContext(tr("RabbitCommon"), szRabbitCommon);
    SetContext(tr("Qt"), szQt);
    if(!szOS.isEmpty())
        SetContext(tr("OS"), szOS);
    if(!szHost.isEmpty())
        SetContext(tr("Host"), szHost);

    //qDebug(RabbitCommon::Logger) << szRabbitCommon << szOS << szQt << szHost;
}

CInformation::~CInformation()
{
    delete ui;
}

void CInformation::SetContext(const QString& szTitle, const QString& szContext)
{
    if(szTitle.isEmpty() || szContext.isEmpty())
    {
        qCritical(RabbitCommon::Logger) << "Title or context is empty";
        return;
    }

#if (defined(HAVE_CMARK) || defined (HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
    QWebEngineView* pEdit = new QWebEngineView(ui->tabWidget);
    if(!pEdit) return;
    pEdit->setHtml(CDlgAbout::MarkDownToHtml(szContext));
    pEdit->show();
    ui->tabWidget->addTab(pEdit, szTitle);
#else
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
#endif
}
