// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#ifndef DLGFILTER_H
#define DLGFILTER_H

#include <QDialog>

namespace Ui {
class CDlgFilter;
}

/*!
 * \brief Set filter dialog
 * \ingroup INTERNAL_API
 */
class CDlgFilter : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgFilter(QWidget *parent = nullptr);
    ~CDlgFilter();
    
    int SetFilter(const QString &szInclude, const QString &szExclude);
    int GetFilter(QString &szInclude, QString &szExclude);
    
private slots:    
    void on_leInclude_editingFinished();
    void on_leExclude_editingFinished();
    
private:
    Ui::CDlgFilter *ui;
};

#endif // DLGFILTER_H
