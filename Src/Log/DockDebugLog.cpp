#include "DockDebugLog.h"
#include "ui_DockDebugLog.h"
#include "TitleBar.h"
#include "Log.h"
#include "RabbitCommonDir.h"
#include <QMenu>
#include <QSettings>
#include <QScrollBar>
#include <QInputDialog>

CDockDebugLog* g_pDcokDebugLog = nullptr;

CDockDebugLog::CDockDebugLog(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CDockDebugLog)
{
    ui->setupUi(this);
    this->hide();
    
    bool check = connect(this, &QDockWidget::visibilityChanged, this, [=](bool visible) {
        if (visible) {
            ui->txtDebugLog->horizontalScrollBar()->setValue(0);
        }
    });
    Q_ASSERT(check);
    check = connect(this, SIGNAL(sigAddLog(QString)),
                    this, SLOT(slotAddLog(QString)));
    Q_ASSERT(check);

    static QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
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
            int nWrap = ui->txtDebugLog->lineWrapMode();
            set.setValue("DockDebugLog/Wrap", nWrap);
        });
        
        int nMaxBlockCount = set.value("DockDebugLog/MaximumBlockCount", ui->txtDebugLog->maximumBlockCount()).toInt();
        ui->txtDebugLog->setMaximumBlockCount(nMaxBlockCount);
        pMenu->addAction(tr("Set maximum block count"), [&](){
            bool ok = false;
            int count = QInputDialog::getInt(this,
                                             tr("Set maximum block count"),
                                             tr("Set maximum block count"),
                                             ui->txtDebugLog->maximumBlockCount(),
                                             0, 100000, 1, &ok);
            if(ok) {
                this->ui->txtDebugLog->setMaximumBlockCount(count);
                set.setValue("DockDebugLog/MaximumBlockCount", ui->txtDebugLog->maximumBlockCount());
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
    g_pDcokDebugLog = nullptr;
    delete ui;
}

int CDockDebugLog::slotAddLog(const QString &szLog)
{
    ui->txtDebugLog->appendPlainText(szLog);
    return 0;
}
