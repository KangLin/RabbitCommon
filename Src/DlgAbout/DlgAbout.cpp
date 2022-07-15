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
#include "RabbitCommonLog.h"

#ifdef HAVE_WebEngineWidgets
    #include <QWebEngineView>
#endif
#include <QTextEdit>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QStandardPaths>
#include <QSslError>
#include <QDateTime>

#ifdef HAVE_CMARK
    #include "cmark.h"
#endif
#ifdef HAVE_CMARK_GFM
    #include "cmark-gfm.h"
    #include "cmark-gfm-core-extensions.h"
    #include "registry.h"
    #include "parser.h"
#endif

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
#ifdef BUILD_VERSION
    if(m_szVersion.isEmpty())
        m_szVersion = BUILD_VERSION;
#endif
    
#ifdef BUILD_ARCH
    m_szArch = BUILD_ARCH;
#endif
    
    m_szDate = __DATE__;
    m_szTime = __TIME__;
    
    m_szAuthor = tr("KangLin");
    m_szEmail = "kl222@126.com";
    m_szHomePage = "https://github.com/KangLin/" + qApp->applicationName();
    m_szCopyrightOwner = tr("Kang Lin Studio");
    m_szCopyrightStartTime = "2019";
    m_szCopyrightTime = tr("%1 - %2").arg(m_szCopyrightStartTime,
                QString::number(QDate::currentDate().year()));
    m_AppIcon = QImage(":/icon/RabbitCommon/App");
    m_CopyrightIcon = QImage(":/icon/RabbitCommon/CopyRight");
    m_DonationIcon = QImage(":/icon/RabbitCommon/Contribute");
    
    DownloadFile(QUrl("https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png"));
    
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
    
#if defined (Q_OS_ANDROID)
    ui->lbDonation->installEventFilter(this);
#else
    bool check = false;
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
    ui->lbVersion->setText(tr("Version: %1 Arch: %2").arg(m_szVersion, m_szArch));
    ui->lbQtVersion->setText(tr("Qt version:") + QT_VERSION_STR);
    ui->lbDate->setText(tr("Build date:%1 %2").arg(m_szDate, m_szTime));
    ui->lbAuthor->setText(tr("Author: ") + m_szAuthor
                          + tr(" Email: ") + "<a href=\"" + m_szEmail + "\">"
                          + m_szEmail + "</a>");
    ui->lbHome->setOpenExternalLinks(true);
    
    ui->lbHome->setText(tr("Home page:") + "<a href=\"" + m_szHomePage + "\">"
                        + m_szHomePage + "</a>");
    if(m_szCopyright.isEmpty())
    {
        m_szCopyrightTime = tr("%1 - %2").arg(m_szCopyrightStartTime,
                    QString::number(QDate::currentDate().year()));
        m_szCopyright = tr("Copyright (C)") + " " + m_szCopyrightTime + " " + m_szCopyrightOwner;
    }
    ui->lbCopyright->setText(m_szCopyright);

    AppendFile(m_pChangeLog, "ChangeLog");
    AppendFile(m_pLicense, "License");
    AppendFile(m_pThanks, "Authors");
    adjustSize();
}

CDlgAbout::~CDlgAbout()
{
    delete ui;
}

int CDlgAbout::AppendFile(QWidget* pWidget, const QString &szFile)
{
    QString szFileLocation;
    
    szFileLocation = RabbitCommon::CDir::Instance()->GetDirApplicationInstallRoot() + QDir::separator()
            + szFile + "_" + RabbitCommon::CTools::Instance()->GetLanguage() + ".md";
    QDir d;
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

void CDlgAbout::on_pushButton_clicked()
{
    close();
}

void CDlgAbout::slotDonation(const QPoint &pos)
{
    Q_UNUSED(pos)
    QMenu m;
    m.addAction(QIcon(":/icon/Save"), tr("Save"),
                this, SLOT(slotSaveDonation()));
    m.exec(QCursor::pos());
}

void CDlgAbout::slotSaveDonation()
{
    QString szDir = RabbitCommon::CDir::Instance()->GetDirUserImage()
            + QDir::separator() + "donation.png";
    QString szFile = RabbitCommon::CDir::GetSaveFileName(this,
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

int CDlgAbout::DownloadFile(const QUrl &url)
{
    int nRet = 0;
    QNetworkRequest request(url);
    //https://blog.csdn.net/itjobtxq/article/details/8244509
    /*QSslConfiguration config;
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    request.setSslConfiguration(config);
    */
    m_pReply = m_NetManager.get(request);
    if(!m_pReply)
        return -1;
    
    bool check = false;
    check = connect(m_pReply,
                #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
                    SIGNAL(errorOccurred(QNetworkReply::NetworkError)),
                #else
                    SIGNAL(error(QNetworkReply::NetworkError)),
                #endif
                    this, SLOT(slotError(QNetworkReply::NetworkError)));
    Q_ASSERT(check);
    check = connect(m_pReply, SIGNAL(sslErrors(const QList<QSslError>&)),
                    this, SLOT(slotSslError(const QList<QSslError>&)));
    check = connect(m_pReply, SIGNAL(finished()),
                    this, SLOT(slotFinished()));
    
    return nRet;
}

void CDlgAbout::slotFinished()
{
    qDebug() << "CFrmUpdater::slotFinished()";
    
    QVariant redirectionTarget;
    if(m_pReply)
        redirectionTarget = m_pReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(redirectionTarget.isValid())
    {
        if(m_pReply)
        {
            m_pReply->disconnect();
            m_pReply->deleteLater();
            m_pReply = nullptr;
        }
        QUrl u = redirectionTarget.toUrl();  
        if(u.isValid())
        {
            qDebug() << "CFrmUpdater::slotFinished():redirectionTarget:url:" << u;
            DownloadFile(u);
        }
        return;
    }
    
    QByteArray d = m_pReply->readAll();
    m_DonationIcon.loadFromData(d);
    ui->lbDonation->setPixmap(QPixmap::fromImage(m_DonationIcon));
    //    QFile f(RabbitCommon::CDir::Instance()->GetDirUserImage()
    //            + QDir::separator() + "donation.png");
    //    if(f.open(QFile::WriteOnly))
    //    {
    //        f.write(d);
    //        f.close();
    //        m_DonationIcon = QPixmap(RabbitCommon::CDir::Instance()->GetDirUserImage()
    //                                 + QDir::separator() + "donation.png", "png");
    //    }
    
    if(m_pReply)
    {
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
}

void CDlgAbout::slotError(QNetworkReply::NetworkError e)
{
    qDebug() << "CFrmUpdater::slotError: " << e;
    if(m_pReply)
    {
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
}

void CDlgAbout::slotSslError(const QList<QSslError> &e)
{
    qDebug() << "CFrmUpdater::slotSslError: " << e;
    
    if(m_pReply)
    {
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
}
