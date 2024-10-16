// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QFile>
#include <QLoggingCategory>
#include "DlgEdit.h"
#include "ui_DlgEdit.h"
#include "RabbitCommonTools.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.DlgEdit")

CDlgEdit::CDlgEdit(const QString &szTitle,
                   const QString &szFile,
                   const QString &szInfo,
                   bool bReadOnly,
                   QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::CDlgEdit)
{
    ui->setupUi(this);
    ui->textEdit->setReadOnly(bReadOnly);
    if(bReadOnly)
        ui->buttonBox->setStandardButtons(QDialogButtonBox::Cancel);
    if(!szTitle.isEmpty())
        setWindowTitle(szTitle);
    if(!szInfo.isEmpty())
        ui->lbFile->setText(szInfo);
    setFile(szFile);
}

CDlgEdit::~CDlgEdit()
{
    qDebug(log) << "CDlgEdit::~CDlgEdit()";
    delete ui;
}

QString CDlgEdit::getContext()
{
    return ui->textEdit->toPlainText();
}

void CDlgEdit::setFile(const QString &szFile)
{
    QString szText;
    if(szFile.isEmpty())
        return;

    QFile file(szFile);
    if(file.open(QFile::ReadOnly)) {
        szText = file.readAll();
        file.close();
    } else {
        qCritical(log) << file.errorString() << szFile;
    }

    if(ui->lbFile->text().isEmpty())
        ui->lbFile->setText(tr("File:") + " " + szFile);
    ui->textEdit->setText(szText);
}
