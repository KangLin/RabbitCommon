#ifndef CTESTUPDATER_H
#define CTESTUPDATER_H

#include <QObject>
#include "FrmUpdater.h"

class CTestUpdater : public QObject
{
    Q_OBJECT
public:
    explicit CTestUpdater(QObject *parent = nullptr);

private Q_SLOTS:
    void TestCheckRedirectJson();
    void TestCheckUpdateJson();
    void TestCheckSingleUpdateJson();
    void TestCheckUnknownUpdateJson();
};

#endif // CTESTUPDATER_H
