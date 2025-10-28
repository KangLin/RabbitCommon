#ifndef EXAMPLEFILE_H
#define EXAMPLEFILE_H

#pragma once

#include <QObject>
#include "ThreadPool.h"

class CExampleFileTransfer : public QObject
{
    Q_OBJECT
public:
    explicit CExampleFileTransfer(QObject *parent = nullptr);

signals:
    
private:
    CThreadPool m_Pool;
};

#endif // EXAMPLEFILE_H
