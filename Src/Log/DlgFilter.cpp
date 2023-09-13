#include "DlgFilter.h"
#include "ui_DlgFilter.h"

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
