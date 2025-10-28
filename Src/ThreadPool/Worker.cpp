// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include <QRandomGenerator>
#include <QTimer>
#include <QLoggingCategory>
#include "Worker.h"

static Q_LOGGING_CATEGORY(log, "Pool.Worker")
CWorker::CWorker(QObject *parent)
    : QObject{parent}
{
    //qDebug(log) << Q_FUNC_INFO;
}

CWorker::~CWorker()
{
    //qDebug(log) << Q_FUNC_INFO;
}

CWorkerTest::CWorkerTest(QObject *parent) : CWorker(parent)
{
    qDebug(log) << Q_FUNC_INFO << this;
    auto t = new QTimer(this);
    t->singleShot(QRandomGenerator::global()->bounded(100), this, &CWorker::sigFinished);
}

CWorkerTest::~CWorkerTest()
{
    qDebug(log) << Q_FUNC_INFO << this;
}

int CWorkerTest::Init()
{
    qDebug(log) << Q_FUNC_INFO << this;
    return 0;
}

int CWorkerTest::Clean()
{
    qDebug(log) << Q_FUNC_INFO << this;
    return 0;
}

int CWorkerTest::Workload()
{
    qDebug(log) << Q_FUNC_INFO << this;
    return 0;
}
