// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "ParameterLog.h"

namespace RabbitCommon {

CParameterLog::CParameterLog(QObject *parent)
    : QObject{parent}
{}

const QString &CParameterLog::GetFilterInclude() const
{
    return m_szFilterInclude;
}

void CParameterLog::SetFilterInclude(const QString &newInclude)
{
    m_szFilterInclude = newInclude;
}

const QString &CParameterLog::GetFilterExclude() const
{
    return m_szFilterExclude;
}

void CParameterLog::SetFilterExclude(const QString &newExclude)
{
    m_szFilterExclude = newExclude;
}

bool CParameterLog::GetOpenFileWithSystemProgram() const
{
    return m_OpenFileWithSystemProgram;
}

void CParameterLog::SetOpenFileWithSystemProgram(bool newOpenFileWithSystemProgram)
{
    m_OpenFileWithSystemProgram = newOpenFileWithSystemProgram;
}

int CParameterLog::OnLoad(const QSettings &set)
{
    int nRet = 0;
    SetFilterInclude(
        set.value("Log/Filter/Include", GetFilterInclude()).toString());
    SetFilterExclude(
        set.value("Log/Filter/Exclude", GetFilterExclude()).toString());
    SetOpenFileWithSystemProgram(
        set.value("Log/OpenFileWithSystemProgram",
                  GetOpenFileWithSystemProgram()).toBool());
    return nRet;
}

int CParameterLog::OnSave(QSettings &set)
{
    int nRet = 0;
    set.setValue("Log/Filter/Include", GetFilterInclude());
    set.setValue("Log/Filter/Exclude", GetFilterExclude());
    set.setValue("Log/OpenFileWithSystemProgram",
                 GetOpenFileWithSystemProgram());
    return nRet;
}

int CParameterLog::Load(const QSettings &set)
{
    return OnLoad(set);
}

int CParameterLog::Save(QSettings &set)
{
    return OnSave(set);
}

} // namespace RabbitCommon