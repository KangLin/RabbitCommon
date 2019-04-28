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
#include "RabbitCommonGlobalDir.h"

#include <QFile>
#include <QDir>
#include <QDebug>

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

#ifdef BUILD_VERSION
    m_szVersion = BUILD_VERSION;
#endif
    
#ifdef BUILD_ARCH
    m_szArch = BUILD_ARCH;
#endif
    
    m_szDate = __DATE__;
    m_szTime = __TIME__;
    
    m_szAuthor = tr("Author: KangLin\nEmail:kl222@126.com");
    m_szHomePage = "https://github.com/KangLin/RabbitCommon";
    m_szCopyright = tr("KangLin Studio");
    
    m_AppIcon = QPixmap(":/icon/RabbitCommon/App");
    m_CopyrightIcon = QPixmap(":/icon/RabbitCommon/CopyRight");
    m_DonationIcon = QPixmap(":/icon/RabbitCommon/Contribute20");
}

void CDlgAbout::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    
    setWindowIcon(m_AppIcon);
    ui->lblLogo->setPixmap(m_AppIcon);
    ui->lbCopyrightIcon->setPixmap(m_CopyrightIcon);
    ui->lbDonation->setPixmap(m_DonationIcon);
    ui->lblName->setText(m_szAppName);
    ui->lbVersion->setText(tr(" Version: ") + m_szVersion + tr(" Arch:") + m_szArch);
    ui->lbDate->setText(tr("Build date:%1 %2").arg(m_szDate, m_szTime));
    ui->lbAuthor->setText(m_szAuthor);
    ui->lbHome->setOpenExternalLinks(true);

    ui->lbHome->setText(tr("Home page:") + "<a href=\"" + m_szHomePage + "\">"
                        + m_szHomePage + "</a>");
    ui->lbCopyright->setText(tr(" Copyright (C) 2018 - %1 %2").arg(
                                 QString::number(QDate::currentDate().year()),
                                 m_szCopyright));

    AppendFile(ui->txtChange, "ChangeLog");
    AppendFile(ui->txtLicense, "License");
    AppendFile(ui->txtThinks, "Authors");
}

CDlgAbout::~CDlgAbout()
{
    delete ui;
}

int CDlgAbout::AppendFile(QTextEdit* pEdit, const QString &szFile)
{
    QString szFileLocation;
#if defined (Q_OS_ANDROID)
    szFileLocation = ":/file/" + szFile + "_" + QLocale().system().name();
    QFile f(szFileLocation);
    if(f.open(QIODevice::ReadOnly))
    {
        f.close();
    } else
        szFileLocation = ":/file/" + szFile;
#else
    szFileLocation = CRabbitCommonGlobalDir::Instance()->GetDirApplication() + QDir::separator()
            + szFile + "_" + QLocale().system().name() + ".md";
    QDir d;
    if(!d.exists(szFileLocation))
        szFileLocation = CRabbitCommonGlobalDir::Instance()->GetDirApplication() 
                + QDir::separator() + szFile + ".md";
#endif
    
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
