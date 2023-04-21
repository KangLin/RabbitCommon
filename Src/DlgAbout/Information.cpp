#include "Information.h"
#include "ui_Information.h"
#include "RabbitCommonTools.h"
#include "DlgAbout.h"

#include <QLibraryInfo>
#include <QHostInfo>
#ifdef HAVE_WebEngineWidgets
#include <QWebEngineView>
#endif
#include <QTextEdit>

CInformation::CInformation(const QString &szApp, const QString &szInfo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CInformation)
{
    ui->setupUi(this);
    ui->tabWidget->removeTab(0);

    QString szRabbitCommon, szOS, szQt, szHost;
    
    szRabbitCommon += tr("### RabbitCommon") + "\n";
    szRabbitCommon += "- " + tr("RabbitCommon version: ") + RabbitCommon::CTools::Version() + "\n";
    
    szQt += tr("### Qt") + "\n";
    szQt += "- " + tr("Qt runtime version: ") + QString(qVersion()) + "\n";
    szQt += "- " + tr("Qt compile version: ") + QString(QT_VERSION_STR) + "\n";
    szQt += "- " + tr("Qt library version: ") + QLibraryInfo::version().toString() + "\n";
    szQt += "- " + tr("Locale: ") + QLocale::system().name() + "\n";
    
    szOS += tr("### OS") + "\n";
    szOS += "- " + tr("OS: ") + QSysInfo::prettyProductName() + "\n";
    szOS += "- " + tr("Kernel type: ") + QSysInfo::kernelType() + "\n";
    szOS += "- " + tr("Kernel version: ") + QSysInfo::kernelVersion() + "\n";
    if(!QSysInfo::bootUniqueId().isEmpty())
        szOS += "- " + tr("Boot Id: ") + QSysInfo::bootUniqueId() + "\n";
    szOS += "- " + tr("Build ABI: ") + QSysInfo::buildAbi() + "\n";
    szOS += "- " + tr("CPU: ") + QSysInfo::currentCpuArchitecture() + "\n";
    szOS += "- " + tr("Build CPU: ") + QSysInfo::buildCpuArchitecture() + "\n";
    
    szHost += tr("### Host") + "\n";
    szHost += "- " + tr("Host name: ") + QSysInfo::machineHostName() + "\n";
    szHost += "- " + tr("Domain name: ") + QHostInfo::localDomainName();

    SetContext(tr("Application"), szApp + szInfo + szRabbitCommon);
    //SetContext(tr("RabbitCommon"), szRabbitCommon);
    SetContext(tr("Qt"), szQt);
    SetContext(tr("OS"), szOS);
    SetContext(tr("Host"), szHost);
}

CInformation::~CInformation()
{
    delete ui;
}

void CInformation::SetContext(const QString& szTitle, const QString& szContext)
{
#if (defined(HAVE_CMARK) || defined (HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
    QWebEngineView* pEdit = new QWebEngineView(ui->tabWidget);
    ui->tabWidget->addTab(pEdit, szTitle);
    pEdit->setHtml(CDlgAbout::MarkDownToHtml(szContext));
#else
    QTextEdit* pEdit = new QTextEdit(ui->tabWidget);
    pEdit->setReadOnly(true);
    pEdit->setWordWrapMode(QTextOption::NoWrap);
    ui->tabWidget->addTab(pEdit, szTitle);
    pEdit->append(szContext);
    //把光标移动文档开始处  
        QTextCursor cursor = pEdit->textCursor();
    cursor.movePosition(QTextCursor::Start);
    pEdit->setTextCursor(cursor);
#endif
}
