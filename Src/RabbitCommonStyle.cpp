// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "RabbitCommonLog.h"
#include "RabbitCommonStyle.h"
#include "RabbitCommonDir.h"

#include <QSettings>
#include <QDebug>
#include <QApplication>
#include <QRegularExpression>
#include <QDir>
#include <QColor>
#include <QPalette>

namespace RabbitCommon {

CStyle::CStyle(QObject *parent) : QObject(parent)
{
    m_szDefaultFile = RabbitCommon::CDir::Instance()->GetDirData(true)
            + QDir::separator()
            + "style" + QDir::separator()
            + "black_green.qss";
    m_szFile = m_szDefaultFile;
}

CStyle* CStyle::Instance()
{
    static CStyle* p = new CStyle();
    if(!p) p = new CStyle();
    return p;
}

void CStyle::SetDefaultFile(const QString &file)
{
    m_szDefaultFile = file;
}

int CStyle::LoadStyle()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QString szFile = set.value("Style/File", m_szDefaultFile).toString();
    qDebug(Logger) << "LoadStyle:" << szFile;
    return  LoadStyle(szFile);
}

int CStyle::LoadStyle(const QString &szFile)
{
    if(szFile.isEmpty())
    {
        qApp->setStyleSheet("");
        m_szFile = szFile;
        //qApp->setPalette(QPalette(QColor(Qt::gray)));
    }
    else
    {
        QFile file(szFile);
        if(file.open(QFile::ReadOnly))
        {
            QString stylesheet= file.readAll();
            QString pattern("QPalette\\{background:#[0-9a-fA-F]+;\\}");
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
            QRegularExpression re(pattern);
            QRegularExpressionMatch match = re.match(stylesheet);
            if (match.hasMatch()) {
                QString matched = match.captured(0);
                if(!matched.isEmpty())
                {
                    QRegularExpression rePalette("#[0-9a-fA-F]+");
                    match = rePalette.match(matched);
                    if(match.hasMatch())
                    {
                        QString paletteColor = match.captured(0);
                        qApp->setPalette(QPalette(QColor(paletteColor)));
                    }
                }            
            }
#else
            QRegExp rx(pattern);
            int pos = rx.indexIn(stylesheet);
            if(pos > -1)
            {
                QString szPalette = rx.cap();
                QRegExp rxPalette("#[0-9a-fA-F]+");
                int pos = rxPalette.indexIn(stylesheet);
                if(pos > -1)
                {
                    QString paletteColor =  rxPalette.cap();
                    qApp->setPalette(QPalette(QColor(paletteColor)));
                }
            }
#endif
            qApp->setStyleSheet(stylesheet);
            file.close();
            m_szFile = szFile;
        }
        else
        {
            qDebug(Logger) << "file open file fail:" << szFile;                       
        }
    }
    return 0;
}

void CStyle::slotSetDefaultStyle()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Style/File", "");
    qApp->setStyleSheet("");
    m_szFile = "";
    //qApp->setPalette(QPalette(QColor(Qt::gray)));
    return;
}

void CStyle::slotStyle()
{
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    QString szFile = set.value("Style/File", m_szDefaultFile).toString();
    if(szFile.isEmpty())
        szFile = m_szDefaultFile;
    QWidget* pParent = dynamic_cast<QWidget*>(this->parent());
    szFile = RabbitCommon::CDir::GetOpenFileName(pParent, tr("Open sink"),
                 szFile,
                 tr("Style files(*.qss *.css);; All files(*.*)"));
    if(szFile.isEmpty()) return;
    LoadStyle(szFile);
    
    set.setValue("Style/File", szFile);
    m_szFile = szFile;
}

QString CStyle::GetStyleFile()
{
    if(!m_szFile.isEmpty())
        if(!QFile::exists(m_szFile))
            return "";
    return m_szFile;
}

} //namespace RabbitCommon
