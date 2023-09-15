#include "DlgFilter.h"
#include "ui_DlgFilter.h"
#include <QDebug>
#include <QRegularExpression>
#include <QMessageBox>

CDlgFilter::CDlgFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CDlgFilter)
{
    ui->setupUi(this);
}

CDlgFilter::~CDlgFilter()
{
    delete ui;
}

int CDlgFilter::SetFilter(const QString &szInclude, const QString &szExclude)
{
    ui->leInclude->setText(szInclude);
    ui->leExclude->setText(szExclude);
    return 0;
}

int CDlgFilter::GetFilter(QString &szInclude, QString &szExclude)
{
    szInclude = ui->leInclude->text();
    szExclude = ui->leExclude->text();
    return 0;
}

void CDlgFilter::on_leInclude_editingFinished()
{
    QRegularExpression r(ui->leInclude->text());
    if(r.isValid())
        return;
    QString szMsg;
    szMsg = tr("Filter of include is error: ") + r.errorString();
    qCritical() << szMsg;
    QMessageBox::critical(this, tr("Error"), szMsg);
}

void CDlgFilter::on_leExclude_editingFinished()
{
    QRegularExpression r(ui->leExclude->text());
    if(r.isValid())
        return;
    QString szMsg;
    szMsg = tr("Filter of exclude is error: ") + r.errorString();
    qCritical() << szMsg;
    QMessageBox::critical(this, tr("Error"), szMsg);
}
