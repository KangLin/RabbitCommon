// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#pragma once

#include <QObject>
#include <QSettings>

namespace RabbitCommon {
class CParameterLog : public QObject
{
    Q_OBJECT
public:
    explicit CParameterLog(QObject *parent = nullptr);

    const QString& GetFilterInclude() const;
    void SetFilterInclude(const QString &newInclude);
    const QString& GetFilterExclude() const;
    void SetFilterExclude(const QString &newExclude);

    bool GetOpenFileWithSystemProgram() const;
    void SetOpenFileWithSystemProgram(bool newOpenFileWithSystemProgram);

    int Load(const QSettings& set);
    int Save(QSettings& set);
    int OnLoad(const QSettings& set);
    int OnSave(QSettings& set);

signals:

private:
    QString m_szFilterInclude;
    QString m_szFilterExclude;

    bool m_OpenFileWithSystemProgram;
};
} // namespace RabbitCommon
