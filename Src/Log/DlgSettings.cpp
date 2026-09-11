// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QRegularExpression>
#include <QMessageBox>
#include <QLoggingCategory>

#include "DlgSettings.h"
#include "ui_DlgSettings.h"

static Q_LOGGING_CATEGORY(log, "RabbitCommon.DlgSettings")
CDlgSettings::CDlgSettings(RabbitCommon::CParameterLog *pPara, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgSettings),
    m_pPara(pPara)
{
    ui->setupUi(this);
    ui->leInclude->setText(m_pPara->GetFilterInclude());
    ui->leExclude->setText(m_pPara->GetFilterExclude());
    ui->cbOpenFile->setChecked(m_pPara->GetOpenFileWithSystemProgram());
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::on_leInclude_editingFinished()
{
    QRegularExpression r(ui->leInclude->text());
    if(r.isValid())
        return;
    QString szMsg;
    szMsg = tr("Filter of include is error: ") + r.errorString();
    qCritical(log) << szMsg;
    QMessageBox::critical(this, tr("Error"), szMsg);
}

void CDlgSettings::on_leExclude_editingFinished()
{
    QRegularExpression r(ui->leExclude->text());
    if(r.isValid())
        return;
    QString szMsg;
    szMsg = tr("Filter of exclude is error: ") + r.errorString();
    qCritical(log) << szMsg;
    QMessageBox::critical(this, tr("Error"), szMsg);
}

void CDlgSettings::accept()
{
    m_pPara->SetFilterInclude(ui->leInclude->text());
    m_pPara->SetFilterExclude(ui->leExclude->text());
    m_pPara->SetOpenFileWithSystemProgram(ui->cbOpenFile->isChecked());
    QDialog::accept();
}
