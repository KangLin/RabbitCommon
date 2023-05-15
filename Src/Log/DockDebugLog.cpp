#include "DockDebugLog.h"
#include "ui_DockDebugLog.h"
#include "TitleBar.h"
#include "Log.h"
#include "RabbitCommonDir.h"
#include <QMenu>
#include <QSettings>

CDockDebugLog* g_pDcokDebugLog = nullptr;

CDockDebugLog::CDockDebugLog(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDockDebugLog)
{
    ui->setupUi(this);
    this->hide();

    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);

    RabbitCommon::CTitleBar* pDockTitleBar = new RabbitCommon::CTitleBar(this);
    if(pDockTitleBar) {
        // Set title widget
        setTitleBarWidget(pDockTitleBar);

        // Create filter pushbutton in title bar
        QMenu* pMenu = new QMenu(tr("Tools"), pDockTitleBar);
        QPushButton* pWrap = pDockTitleBar->CreateSmallPushButton(
            QIcon::fromTheme("tools"), pDockTitleBar);
        pWrap->setToolTip(tr("Wrap"));
        pWrap->setMenu(pMenu);
        QList<QWidget*> lstWidget;
        lstWidget << pWrap;
        pDockTitleBar->AddWidgets(lstWidget);

        QString szTitle;

        szTitle = tr("Clear");
        pMenu->addAction(QIcon::fromTheme("edit-clear"), szTitle, [&](){
            ui->txtDebugLog->clear(); 
        });

        szTitle = tr("Wrap");
        QIcon iconWrap;
        int nWrap = QPlainTextEdit::NoWrap;
        nWrap = set.value("DockDebugLog/Wrap", nWrap).toInt();
        if(nWrap == QPlainTextEdit::NoWrap)
        {
            szTitle = tr("Wrap");
            iconWrap = QIcon::fromTheme("text-wrap");
        } else {   
            szTitle = tr("No wrap");
            iconWrap = QIcon::fromTheme("format-justify-fill");
        }
        ui->txtDebugLog->setLineWrapMode((QPlainTextEdit::LineWrapMode)nWrap);
        pMenu->addAction(iconWrap, szTitle, this, [&](){
            QAction* p = qobject_cast<QAction*>(sender());
            p->setCheckable(true);
            if(ui->txtDebugLog->lineWrapMode() == QPlainTextEdit::NoWrap)
            {
                p->setText(tr("No wrap"));
                p->setIcon(QIcon::fromTheme("format-justify-fill"));
                p->setChecked(true);
                ui->txtDebugLog->setLineWrapMode(QPlainTextEdit::WidgetWidth);
            } else {
                p->setText(tr("Wrap"));
                p->setIcon(QIcon::fromTheme("text-wrap"));
                p->setChecked(false);
                ui->txtDebugLog->setLineWrapMode(QPlainTextEdit::NoWrap);
            }
        });

        pMenu->addSeparator();
        pMenu->addAction(QIcon::fromTheme("document-open"),
                         QObject::tr("Open Log configure file"),
                         [](){RabbitCommon::OpenLogConfigureFile();});
        pMenu->addAction(QIcon::fromTheme("folder-open"),
                         tr("Open log folder"), [](){
           RabbitCommon::OpenLogFolder();
        });
    }
}

CDockDebugLog::~CDockDebugLog()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    int nWrap = ui->txtDebugLog->lineWrapMode();
    set.setValue("DockDebugLog/Wrap", nWrap);
    g_pDcokDebugLog = nullptr;
    delete ui;
}

int CDockDebugLog::AddLog(const QString &szLog)
{
    ui->txtDebugLog->appendPlainText(szLog);
    return 0;
}
