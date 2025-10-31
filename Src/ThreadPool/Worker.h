// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once
#include <QObject>
#include "rabbitcommon_export.h"

class RABBITCOMMON_EXPORT CWorker : public QObject
{
    Q_OBJECT
public:
    explicit CWorker(QObject *parent = nullptr);
    virtual ~CWorker() override;

    virtual int Init() = 0;
    virtual int Clean() = 0;
    
    /*
     * \~chinese 工作负载
     * \~english Workload
     */
    virtual int Workload() = 0;
    
Q_SIGNALS:
    void sigFinished();
};

class CWorkerTest : public CWorker
{
    Q_OBJECT
public:
    explicit CWorkerTest(QObject* parent = nullptr);
    virtual ~CWorkerTest() override;

    virtual int Init() override;
    virtual int Clean() override;
    virtual int Workload() override;
};
