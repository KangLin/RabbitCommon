// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QDialog>
#include "ParameterLog.h"

namespace Ui {
class CDlgSettings;
}

/*!
 * \brief Set filter dialog
 * \ingroup INTERNAL_API
 */
class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(RabbitCommon::CParameterLog* pPara,
                           QWidget *parent = nullptr);
    ~CDlgSettings();

private slots:    
    void on_leInclude_editingFinished();
    void on_leExclude_editingFinished();

private:
    Ui::CDlgSettings *ui;
    RabbitCommon::CParameterLog* m_pPara;
    // QDialog interface
public slots:
    virtual void accept() override;
};
