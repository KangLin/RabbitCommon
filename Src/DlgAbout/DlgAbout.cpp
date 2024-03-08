/*++
Copyright (c) Kang Lin studio, All Rights Reserved

Author:
    Kang Lin <kl222@126.com>
    
Module Name:

    DlgAbout.cpp
    
Abstract:

    This file contains about dialog implement.
 */

#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"

#include "Log/Log.h"
#include "Information.h"

#ifdef HAVE_WebEngineWidgets
    #include <QWebEngineView>
#endif
#include <QTextEdit>
#include <QFile>
#include <QDir>
#include <QMenu>
#include <QStandardPaths>
#include <QSslError>
#include <QDateTime>
#include <QCursor>
#include <QLoggingCategory>
#include <QApplication>

#ifdef HAVE_CMARK
    #include "cmark.h"
#endif
#ifdef HAVE_CMARK_GFM
    #include "cmark-gfm.h"
    #include "cmark-gfm-core-extensions.h"
    #include "registry.h"
    #include "parser.h"
#endif

static Q_LOGGING_CATEGORY(log, "RabbitCommon.DlgAbout")

CDlgAbout::CDlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAbout)
{
#ifdef HAVE_CMARK_GFM
    cmark_gfm_core_extensions_ensure_registered();
    // free extensions at application exit (cmark-gfm is not able to register/unregister more than once)
    std::atexit(cmark_release_plugins);
#endif

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
    m_DonationIcon = QImage(":/icon/RabbitCommon/Contribute");
    
    QVector<QUrl> urls;
    urls << QUrl("https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png")
         << QUrl("https://gitee.com/kl222/RabbitCommon/raw/master/Src/Resource/image/Contribute.png")
         << QUrl("https://gitlab.com/kl222/RabbitCommon/-/raw/master/Src/Resource/image/Contribute.png");
    m_Download = QSharedPointer<RabbitCommon::CDownload>(
        new RabbitCommon::CDownload(), &QObject::deleteLater);
    bool check = connect(m_Download.data(), SIGNAL(sigFinished(const QString)),
                         this, SLOT(slotDownloadFileFinished(const QString)));
    Q_ASSERT(check);
    check = connect(m_Download.data(), SIGNAL(sigError(int, const QString)),
                    this, SLOT(slotDownloadError(int, const QString)));
    Q_ASSERT(check);
    m_Download->Start(urls);

#if (defined(HAVE_CMARK) || defined (HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
    m_pLicense = new QWebEngineView(ui->tabWidget);
    ui->tabWidget->addTab(m_pLicense, tr("License"));
    m_pChangeLog = new QWebEngineView(ui->tabWidget);
    ui->tabWidget->addTab(m_pChangeLog, tr("Change log"));
    m_pThanks = new QWebEngineView(ui->tabWidget);
    ui->tabWidget->addTab(m_pThanks, tr("Thanks"));
#else
    m_pLicense = new QTextEdit(ui->tabWidget);
    qobject_cast<QTextEdit*>(m_pLicense)->setReadOnly(true);
    ui->tabWidget->addTab(m_pLicense, tr("License"));
    m_pChangeLog = new QTextEdit(ui->tabWidget);
    qobject_cast<QTextEdit*>(m_pChangeLog)->setReadOnly(false);
    ui->tabWidget->addTab(m_pChangeLog, tr("Change log"));
    m_pThanks = new QTextEdit(ui->tabWidget);
    qobject_cast<QTextEdit*>(m_pThanks)->setReadOnly(false);
    ui->tabWidget->addTab(m_pThanks, tr("Thanks"));
#endif

    AppendFile(m_pChangeLog, "ChangeLog");
    AppendFile(m_pLicense, "License");
    AppendFile(m_pThanks, "Authors");

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

int CDlgAbout::AppendFile(QWidget* pWidget, const QString &szFile)
{
    QDir d;
    QString szFileLocation;

    szFileLocation = RabbitCommon::CDir::Instance()->GetDirData()
                     + QDir::separator() + "doc" + QDir::separator()
                     + QApplication::applicationName() + QDir::separator()
                     + szFile + "_" + RabbitCommon::CTools::Instance()->GetLanguage() + ".md";
    if(!d.exists(szFileLocation))
        szFileLocation = RabbitCommon::CDir::Instance()->GetDirData()
                         + QDir::separator() + "doc" + QDir::separator()
                         + QApplication::applicationName() + QDir::separator()
                         + szFile + ".md";
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
#if (defined(HAVE_CMARK) || defined(HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
        QWebEngineView* pEdit = qobject_cast<QWebEngineView*>(pWidget);
        QString szText = MarkDownToHtml(text);
        if(szText.isEmpty())
            szText = text;
        pEdit->setHtml(szText);
#else
        QTextEdit* pEdit = qobject_cast<QTextEdit*>(pWidget);
        pEdit->append(text);
        //把光标移动文档开始处  
        QTextCursor cursor = pEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        pEdit->setTextCursor(cursor);
#endif
        readme.close();
    }
    return 0;
}

QString CDlgAbout::MarkDownToHtml(const QString &szText)
{
    QString szRetureText = szText;

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
            szRetureText = rendered_html;
            free(rendered_html);
        }
        cmark_node_free(doc);
    }
    cmark_llist_free(mem, syntax_extensions);
    cmark_parser_free(parser);

#elif HAVE_CMARK

    char* pHtml = cmark_markdown_to_html(szText.toStdString().c_str(),
                                         szText.toStdString().length(),
                                         0);
    if(pHtml)
    {
        szRetureText = pHtml;
        free(pHtml);
    }

#endif
    return szRetureText;
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
#if (defined(HAVE_CMARK) || defined(HAVE_CMARK_GFM)) && defined(HAVE_WebEngineWidgets)
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

    szApp  = tr("### ") + QApplication::applicationDisplayName() + "\n";
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
    info.exec();
    setCursor(cursor);
}
