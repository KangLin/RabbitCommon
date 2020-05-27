/*++
Copyright (c) Kang Lin studio, All Rights Reserved

Author:
    Kang Lin(kl222@126.com）
    
Module Name:

    DlgAbout.cpp
    
Abstract:

    This file contains about dialog implement.
 */

#include "DlgAbout.h"
#include "ui_DlgAbout.h"
#include "RabbitCommonDir.h"
#include "RabbitCommonTools.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QStandardPaths>
#include <QtNetwork>

/*
 * Author: KangLin(Email:kl222@126.com)
 */
#include <QDateTime>

CDlgAbout::CDlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgAbout)
{
    ui->setupUi(this);
    
    setAttribute(Qt::WA_QuitOnClose, false);
    
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
    
    m_szAuthor = tr("Author: KangLin\nEmail:kl222@126.com");
    m_szHomePage = "https://github.com/KangLin/RabbitCommon";
    m_szCopyrightOwner = tr("Kang Lin Studio");
    m_szCopyrightTime = tr("2018 - %1 ").arg(
                QString::number(QDate::currentDate().year()));
    m_AppIcon = QImage(":/icon/RabbitCommon/App");
    m_CopyrightIcon = QImage(":/icon/RabbitCommon/CopyRight");
    m_DonationIcon = QImage(":/icon/RabbitCommon/Contribute");
    
    DownloadFile(QUrl("https://github.com/KangLin/RabbitCommon/raw/master/Src/Resource/image/Contribute.png"));
    
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
    ui->lbVersion->setText(tr(" Version: ") + m_szVersion + tr(" Arch:") + m_szArch);
    ui->lbDate->setText(tr("Build date:%1 %2").arg(m_szDate, m_szTime));
    ui->lbAuthor->setText(m_szAuthor);
    ui->lbHome->setOpenExternalLinks(true);
    
    ui->lbHome->setText(tr("Home page:") + "<a href=\"" + m_szHomePage + "\">"
                        + m_szHomePage + "</a>");
    if(m_szCopyright.isEmpty())
        m_szCopyright = tr("Copyright (C) ") + m_szCopyrightTime + m_szCopyrightOwner;
    ui->lbCopyright->setText(m_szCopyright);
    
    AppendFile(ui->txtChange, "ChangeLog");
    AppendFile(ui->txtLicense, "License");
    AppendFile(ui->txtThinks, "Authors");
    adjustSize();
}

CDlgAbout::~CDlgAbout()
{
    delete ui;
}

int CDlgAbout::AppendFile(QTextEdit* pEdit, const QString &szFile)
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
        pEdit->append(readme.readAll());
        //把光标移动文档开始处  
        QTextCursor cursor =   pEdit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        pEdit->setTextCursor(cursor);
        readme.close();
    }
    return 0;
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
    check = connect(m_pReply, SIGNAL(error(QNetworkReply::NetworkError)),
                    this, SLOT(slotError(QNetworkReply::NetworkError)));
    Q_ASSERT(check);
    check = connect(m_pReply, SIGNAL(sslErrors(const QList<QSslError>)),
                    this, SLOT(slotSslError(const QList<QSslError>)));
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

void CDlgAbout::slotSslError(const QList<QSslError> e)
{
    qDebug() << "CFrmUpdater::slotSslError: " << e;
    
    if(m_pReply)
    {
        m_pReply->disconnect();
        m_pReply->deleteLater();
        m_pReply = nullptr;
    }
}
