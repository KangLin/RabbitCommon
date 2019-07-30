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
    QString m_szHomePage;
    QString m_szCopyright;
    QPixmap m_AppIcon;
    QPixmap m_CopyrightIcon;
    QPixmap m_DonationIcon;

#if defined(Q_OS_ANDROID)
Q_SIGNALS:
    void sigDonationClicked();
protected:    
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
#endif

private slots:
    void on_pushButton_clicked();
    void slotDonation(const QPoint &pos);
    void slotSaveDonation();
    
private:
    Ui::CDlgAbout *ui;
        
    int AppendFile(QTextEdit *pEdit, const QString &szFile);

protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // RABBITCOMMON_DLGABOUT_H
