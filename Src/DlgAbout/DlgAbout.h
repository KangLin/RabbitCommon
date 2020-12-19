/*++
Copyright (c) Kang Lin studio, All Rights Reserved

Author:
    Kang Lin(kl222@126.com）

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
#include <QTextEdit>
#include "rabbitcommon_export.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>

namespace Ui {
class CDlgAbout;
}

class RABBITCOMMON_EXPORT CDlgAbout : public QDialog
{
    Q_OBJECT

public:
    explicit CDlgAbout(QWidget *parent = nullptr);
    virtual ~CDlgAbout() override;

public:
    QString m_szAppName;
    QString m_szVersion;
    QString m_szArch;
    QString m_szDate, m_szTime;
    QString m_szAuthor;
    QString m_szEmail;
    QString m_szHomePage;
    QString m_szCopyright;
    QString m_szCopyrightTime;
    QString m_szCopyrightStartTime;
    QString m_szCopyrightOwner;
    QImage m_AppIcon;
    QImage m_CopyrightIcon;
    QImage m_DonationIcon;

#if defined(Q_OS_ANDROID)
protected:    
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
#endif

protected Q_SLOTS:
    void on_pushButton_clicked();
    void slotDonation(const QPoint &pos);
    void slotSaveDonation();
    
    void slotError(QNetworkReply::NetworkError e);
    void slotSslError(const QList<QSslError> e);
    void slotFinished();
    
private:
    Ui::CDlgAbout *ui;

    QNetworkAccessManager m_NetManager;
    QNetworkReply *m_pReply;
    /**
     * @brief DownloadFile
     * @param url: Download url
     * @param bRedirection: true: Is redirection
     * @param bDownload: true: don't check, download immediately
     * @return 
     */
    int DownloadFile(const QUrl &url);
    
    int AppendFile(QTextEdit *pEdit, const QString &szFile);

protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // RABBITCOMMON_DLGABOUT_H
