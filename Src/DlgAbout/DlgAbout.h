/*++
Copyright (c) Kang Lin studio, All Rights Reserved

Author:
    Kang Lin(kl222@126.com）

Module Name:

    DlgAbout.h

Abstract:

    This file contains about dialog define.
 */

#ifndef DLGABOUT_H
#define DLGABOUT_H

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
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::CDlgAbout *ui;
        
    int AppendFile(QTextEdit *pEdit, const QString &szFile);

protected:
    virtual void showEvent(QShowEvent *event) override;
};

#endif // DLGABOUT_H
