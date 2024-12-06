/*
Copyright (c) Kang Lin studio, All Rights Reserved

Author:
    Kang Lin <kl222@126.com>

Module Name:

    DlgAbout.h

Abstract:

    About dialog
    
    User must install the follow file to install root:
    ChangeLog:  ChangeLog.md or ChangeLog[_locale].md
    License:    License.md or License[_locale].md
    Authors:    Authors.md or Authors[_locale].md
    
    locale is the language used. eg:
    Chines language: ChangeLog_zh_CN.md     
    English language: ChangeLog_en.md
        
 */

#ifndef RABBITCOMMON_DLGABOUT_H
#define RABBITCOMMON_DLGABOUT_H

#include <QDialog>

#include "rabbitcommon_export.h"

namespace Ui {
class CDlgAbout;
}

namespace RabbitCommon {
class CDownload;
}

/*!
 * \~chinese
 * \brief 关于对话框
 * \details
 *     用户必须安装下列文件到 ${CMAKE_INSTALL_DOCDIR}
 *     \li ChangeLog:  ChangeLog.md or ChangeLog[_locale].md
 *     \li License:    License.md or License[_locale].md
 *     \li Authors:    Authors.md or Authors[_locale].md
 *
 *    locale 是使用的语言。例如:
 *    \li 中文: ChangeLog_zh_CN.md
 *    \li 英文: ChangeLog_en.md
 *
 * \~english
 * \brief About dialog
 * \details User must install the follow file to ${CMAKE_INSTALL_DOCDIR}:
 *    \li ChangeLog:  ChangeLog.md or ChangeLog[_locale].md
 *    \li License:    License.md or License[_locale].md
 *    \li Authors:    Authors.md or Authors[_locale].md
 *
 *    locale is the language used. eg:
 *       \li Chines language: ChangeLog_zh_CN.md
 *       \li English language: ChangeLog_en.md
 * \~
 * \see 
 *   - [GNU installation Directories in cmake](https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html#module:GNUInstallDirs)
 *   - [GNU installation Directories](https://www.gnu.org/prep/standards/html_node/Directory-Variables.html)
 *
 * \ingroup API
 */
class RABBITCOMMON_EXPORT CDlgAbout : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgAbout(QWidget *parent = nullptr);
    virtual ~CDlgAbout() override;

public:
    QString m_szAppName;
    QImage m_AppIcon;
    //! The application version, it is different RabbitCommon::CTools::Version()
    QString m_szVersion;
    QString m_szVersionRevision;
    QString m_szVersionRevisionUrl;
    QString m_szArch;
    QString m_szBuildTime;
    QString m_szDetails; // The applaction's information append to details dialog
    QString m_szAuthor;
    QString m_szEmail;
    QString m_szHomePage;
    QString m_szCopyright;
    QImage m_CopyrightIcon;
    QString m_szCopyrightTime;
    QString m_szCopyrightStartTime;
    QString m_szCopyrightOwner;
    int SetDonationIcon(const QImage& img);

#if defined(Q_OS_ANDROID)
protected:    
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
#endif

protected Q_SLOTS:
    void on_pbOK_clicked();
    void on_pbDetails_clicked();
    void slotDonation(const QPoint &pos);
    void slotSaveDonation();

    void slotDownloadError(int nErr, const QString szError);
    void slotDownloadFileFinished(const QString szFile);
    
private:
    Ui::CDlgAbout *ui;
    
    QImage m_DonationIcon;
    QSharedPointer<RabbitCommon::CDownload> m_Download;

    QString BuildTime();
    QString Version();

    QWidget* m_pLicense;
    QWidget* m_pChangeLog;
    QWidget* m_pThanks;
    int AppendFile(QWidget *pWidget, const QString &szFile, const QString &szTitle);
    static QString MarkDownToHtml(const QString &szText);
    friend class CInformation;
    
protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // RABBITCOMMON_DLGABOUT_H
