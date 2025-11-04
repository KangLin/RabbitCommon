/*++
Copyright (c) Kang Lin studio, All Rights Reserved

Author:
    Kang Lin <kl222@126.com>
    
Module Name:

    DlgAbout.cpp
    
Abstract:

    This file contains about dialog implement.
 */

#include <QFile>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>
#include <QSslError>
#include <QDateTime>
#include <QCursor>
#include <QLoggingCategory>
#include <QApplication>
#ifdef HAVE_WebEngineWidgets
#include <QWebEngineView>
#endif

#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"
#include "Download.h"
#include "Information.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.DlgAbout")

CDlgAbout::CDlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAbout),
    m_pLicense(nullptr),
    m_pChangeLog(nullptr),
    m_pThanks(nullptr)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_QuitOnClose, true);

    m_szAppName = qApp->applicationDisplayName();

    m_szVersion = qApp->applicationVersion();
#ifdef RabbitCommon_VERSION
    if(m_szVersion.isEmpty())
    {
        qWarning(log) << "The m_szVersion is not set. Please use qApp->setApplicationVersion() or m_szVersion";
        m_szVersion = RabbitCommon_VERSION;
    }
#endif

    m_szArch = QSysInfo::buildCpuArchitecture();

    m_szAuthor = tr("KangLin");
    m_szEmail = "kl222@126.com";
    m_szHomePage = "https://github.com/KangLin/" + qApp->applicationName();
    m_szCopyrightOwner = tr("Kang Lin Studio");
    m_szCopyrightStartTime = "2019";
    m_AppIcon = QImage(":/icon/RabbitCommon/App");
    m_CopyrightIcon = QImage(":/icon/RabbitCommon/CopyRight");

    QString szLangUage = RabbitCommon::CTools::Instance()->GetLanguage();
    QVector<QUrl> urls;
    if("zh_CN" == szLangUage) {
        m_DonationIcon = QImage(":/icon/RabbitCommon/Contribute_zh_CN");
        urls << QUrl("https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute_zh_CN.png")
             << QUrl("https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute_zh_CN.png")
             << QUrl("https://gitlab.com/kl222/RabbitCommon/-/raw/master/Src/Resource/image/Contribute_zh_CN.png");
    } else {
        m_DonationIcon = QImage(":/icon/RabbitCommon/Contribute_en");
        urls << QUrl("https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute_en.png")
             << QUrl("https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute_en.png")
             << QUrl("https://gitlab.com/kl222/RabbitCommon/-/raw/master/Src/Resource/image/Contribute_en.png");
    }
    m_Download = QSharedPointer<RabbitCommon::CDownload>(
        new RabbitCommon::CDownload(), &QObject::deleteLater);
    bool check = connect(m_Download.data(), SIGNAL(sigFinished(const QString)),
                         this, SLOT(slotDownloadFileFinished(const QString)));
    Q_ASSERT(check);
    check = connect(m_Download.data(), SIGNAL(sigError(int, const QString)),
                    this, SLOT(slotDownloadError(int, const QString)));
    Q_ASSERT(check);
    m_Download->Start(urls);

    m_pLicense = new QTextBrowser(ui->tabWidget);
    m_pChangeLog = new QTextBrowser(ui->tabWidget);
    m_pThanks = new QTextBrowser(ui->tabWidget);

    AppendFile(m_pChangeLog, "ChangeLog", tr("Change log"));
    AppendFile(m_pLicense, "License", tr("License"));
    AppendFile(m_pThanks, "Authors", tr("Thanks"));

#if defined (Q_OS_ANDROID)
    ui->lbDonation->installEventFilter(this);
#else
    ui->lbDonation->setContextMenuPolicy(Qt::CustomContextMenu);
    check = connect(ui->lbDonation, SIGNAL(customContextMenuRequested(const QPoint &)),
                    this, SLOT(slotDonation(const QPoint &)));
    Q_ASSERT(check);
#endif

}

void CDlgAbout::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    
    setWindowIcon(QIcon(QPixmap::fromImage(m_AppIcon)));
    ui->lblLogo->setPixmap(QPixmap::fromImage(m_AppIcon));
    ui->lbCopyrightIcon->setPixmap(QPixmap::fromImage(m_CopyrightIcon));
    ui->lbDonation->setPixmap(QPixmap::fromImage(m_DonationIcon));
    ui->lblName->setText(m_szAppName);
    ui->lbVersion->setText(Version());
    ui->lbVersion->setOpenExternalLinks(true);
    ui->lbAuthor->setText(tr("Author: ") + m_szAuthor
                          + tr(" Email: ") + "<a href=\"" + m_szEmail + "\">"
                          + m_szEmail + "</a>");
    ui->lbAuthor->setOpenExternalLinks(true);
    ui->lbHome->setOpenExternalLinks(true);
    ui->lbHome->setText(tr("Home page: ") + "<a href=\"" + m_szHomePage + "\">"
                        + m_szHomePage + "</a>");
    if(m_szCopyright.isEmpty())
    {
        if(m_szCopyrightTime.isEmpty())
            m_szCopyrightTime = tr("%1 - %2").arg(m_szCopyrightStartTime,
                    QString::number(QDate::currentDate().year()));
        m_szCopyright = tr("Copyright (C)") + " " + m_szCopyrightTime + " " + m_szCopyrightOwner;
    }
    ui->lbCopyright->setText(m_szCopyright);

    adjustSize();
}

CDlgAbout::~CDlgAbout()
{
    delete ui;
}

int CDlgAbout::AppendFile(QTextBrowser *pEdit, const QString &szFile, const QString &szTitle)
{
    QDir d;
    QString szFileLocation;
    if(!pEdit) return -1;
    szFileLocation = RabbitCommon::CDir::Instance()->GetDirDocument(
                         QApplication::applicationName(), true)
                     + QDir::separator()
                     + szFile + "_" + RabbitCommon::CTools::Instance()->GetLanguage() + ".md";
    if(!d.exists(szFileLocation))
        szFileLocation = RabbitCommon::CDir::Instance()->GetDirDocument(
                             QApplication::applicationName(), true)
                         + QDir::separator()
                         + szFile + ".md";
    //TODO: be will remove in 3.0
    if(!d.exists(szFileLocation))
        szFileLocation = RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot() + QDir::separator()
            + szFile + "_" + RabbitCommon::CTools::Instance()->GetLanguage() + ".md";
    if(!d.exists(szFileLocation))
        szFileLocation = RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot()
                + QDir::separator() + szFile + ".md";

    QFile readme(szFileLocation);
    if(readme.open(QFile::ReadOnly))
    {
        QByteArray text;
        text = readme.readAll();
        QString szHtml;
        #if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        szHtml = RabbitCommon::CTools::MarkDownToHtml(text);
        #endif
        if(szHtml.isEmpty())
            szHtml = text;
        if(pEdit) {
            pEdit->setOpenExternalLinks(true);
            pEdit->setOpenLinks(true);
            pEdit->setReadOnly(true);
            #if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
            pEdit->setMarkdown(text);
            #else
            pEdit->setHtml(szHtml);
            #endif
            //把光标移动文档开始处
            QTextCursor cursor = pEdit->textCursor();
            cursor.movePosition(QTextCursor::Start);
            pEdit->setTextCursor(cursor);
            pEdit->show();
        }
        readme.close();
        if(pEdit)
            ui->tabWidget->addTab(pEdit, szTitle);
    }
    return 0;
}

void CDlgAbout::on_pbOK_clicked()
{
    close();
}

void CDlgAbout::slotDonation(const QPoint &pos)
{
    Q_UNUSED(pos)
    QMenu m;
    m.addAction(QIcon::fromTheme("document-save"), tr("Save"),
                this, SLOT(slotSaveDonation()));
    m.exec(QCursor::pos());
}

void CDlgAbout::slotSaveDonation()
{
    QString szDir = RabbitCommon::CDir::Instance()->GetDirUserImage()
            + QDir::separator() + "donation.png";
    QString szFile =  QFileDialog::getSaveFileName(this,
                                           tr("Save donation picture"),
                                           szDir,
                                           tr("Images (*.png *.xpm *.jpg)"));
    QFileInfo fi(szFile);
    if(fi.suffix().isEmpty())
        szFile += ".png";
    if(!szFile.isEmpty())
        m_DonationIcon.save(szFile);
}

#if defined (Q_OS_ANDROID)
bool CDlgAbout::eventFilter(QObject *watched, QEvent *event)
{
    qDebug() << event->type();
    if(event->type() ==  QEvent::MouseButtonRelease)
        slotSaveDonation();
    return QDialog::eventFilter(watched, event);
}
#endif

int CDlgAbout::SetDonationIcon(const QImage &img)
{
    m_Download.clear();
    m_DonationIcon = img;
    return 0;
}

QString CDlgAbout::BuildTime()
{
    if(m_szBuildTime.isEmpty())
        return QString("%1/%2").arg(__DATE__, __TIME__);
    return m_szBuildTime;
}

QString CDlgAbout::Version()
{
    QString szVersion;
    if(m_szVersionRevision.isEmpty())
    {
        szVersion = tr("Version: ") + m_szVersion + " " + tr("Arch: ") + m_szArch;
    } else {
        if(m_szVersionRevisionUrl.isEmpty())
        {
            m_szVersionRevisionUrl =
                    m_szHomePage + "/tree/" + m_szVersionRevision;
        }
        szVersion = tr("Version: ") + m_szVersion + tr(" (From revision: ")
#if (defined(HAVE_CMARK) || defined(HAVE_CMARK_GFM))
                    + "<a href=\"" + m_szVersionRevisionUrl + "\">"
                    + m_szVersionRevision + "</a>"
#else
                    + m_szVersionRevision
#endif
                    + ") " + tr("Arch: ") + m_szArch;
    }
    return szVersion;
}

void CDlgAbout::slotDownloadError(int nErr, const QString szError)
{
    QString szMsg;
    szMsg = tr("Failed:") + tr("Download file is Failed.");
    if(!szError.isEmpty())
        szMsg += "(" + szError + ")";
    qCritical(log) << szMsg << nErr;
}

void CDlgAbout::slotDownloadFileFinished(const QString szFile)
{
    qDebug(log) << "slotDownloadFileFinished:" << szFile;
    m_DonationIcon.load(szFile);
    ui->lbDonation->setPixmap(QPixmap::fromImage(m_DonationIcon));
}

void CDlgAbout::on_pbDetails_clicked()
{
    QString szApp;

    szApp  = "# " + QApplication::applicationDisplayName() + "\n";
    szApp += "- " + Version() + "\n";
    szApp += "- " + tr("Build Date/Time: ") + BuildTime() + "\n";
    szApp += "- " + tr("File Path: ") + QApplication::applicationFilePath() + "\n";
    szApp += "- " + tr("Arguments: ") + qApp->arguments().join(' ') + "\n";

    QCursor cursor = this->cursor();
    setCursor(Qt::WaitCursor);
    CInformation info(szApp, m_szDetails, this);
#if defined (Q_OS_ANDROID)
    info.showMaximized();
#endif
    RC_SHOW_WINDOW(&info);
    setCursor(cursor);
}
