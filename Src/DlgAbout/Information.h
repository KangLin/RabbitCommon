// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#ifndef INFORMATION_H
#define INFORMATION_H

#include <QDialog>
#include <QLibraryInfo>

namespace Ui {
class CInformation;
}

class CInformation : public QDialog
{
    Q_OBJECT
    
public:
    explicit CInformation(const QString &szApp,
                          const QString &szInfo,
                          QWidget *parent = nullptr);
    virtual ~CInformation();

private:
    Ui::CInformation *ui;

    void SetContext(const QString& szTitle, const QString& szContext);
    QString GetLibrariesLocation();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QString GetLibrariesLocation(QLibraryInfo::LibraryLocation path);
#else
    QString GetLibrariesLocation(QLibraryInfo::LibraryPath path);
#endif
};

#endif // INFORMATION_H
