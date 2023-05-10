// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "Style.h"
#include "RabbitCommonDir.h"

#include <QSettings>
#include <QDebug>
#include <QApplication>
#include <QRegularExpression>
#include <QDir>
#include <QColor>
#include <QPalette>
#include <QIcon>

namespace RabbitCommon {

Q_LOGGING_CATEGORY(LoggerStyle, "RabbitCommon.Style")

CStyle::CStyle(QObject *parent) : QObject(parent)
{
    m_szDefaultFile = QString(); /*RabbitCommon::CDir::Instance()->GetDirData(true)
            + QDir::separator()
            + "style" + QDir::separator()
            + "white.qss"; //TODO: can modify default style //*/

    m_szDefaultIconTheme = QIcon::themeName();
    if(m_szDefaultIconTheme.isEmpty())
        m_szDefaultIconTheme = "black"; //TODO: can modify default icon theme

#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
    if(!QIcon::fallbackThemeName().isEmpty())
        m_szDefaultFallbackIconTheme = QIcon::fallbackThemeName();
    if(m_szDefaultFallbackIconTheme.isEmpty())
        m_szDefaultFallbackIconTheme = "black"; //TODO: can modify default fallback icon theme
#endif

}

CStyle* CStyle::Instance()
{
    static CStyle* p = new CStyle();
    if(!p) p = new CStyle();
    return p;
}

void CStyle::SetFile(const QString &file)
{
    m_szFile = file;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    set.setValue("Style/File", m_szFile);
    LoadStyle(m_szFile);
}

int CStyle::LoadStyle()
{
    // Load icons theme
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    bool bIconTheme = set.value("Style/Icon/Theme/Enable", true).toBool();
    if(bIconTheme) {
        QIcon::setThemeSearchPaths(QIcon::themeSearchPaths()
                                   << RabbitCommon::CDir::Instance()->GetDirIcons(true));
        QString szThemeName = QIcon::themeName();
        if(szThemeName.isEmpty())
            szThemeName = m_szDefaultIconTheme;
        QIcon::setThemeName(set.value("Style/Icon/Theme",
                                      szThemeName).toString());
        
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
        QIcon::setFallbackSearchPaths(QIcon::fallbackSearchPaths()
                                      << RabbitCommon::CDir::Instance()->GetDirIcons(true));
        QString szFallbackThemeName = QIcon::fallbackThemeName();
        if(szFallbackThemeName.isEmpty())
            szFallbackThemeName = m_szDefaultFallbackIconTheme;
        QIcon::setFallbackThemeName(set.value("Style/Icon/Theme/Fallback",
                                              szFallbackThemeName).toString());
#endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        
        qDebug(LoggerStyle) << "Icon theme search paths:" << QIcon::themeSearchPaths()
                            << "Icon theme name:" << QIcon::themeName()
                       #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                            << "Fallback search paths:" << QIcon::fallbackSearchPaths()
                       #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                       #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                            << "Fallback theme name:" << QIcon::fallbackThemeName()
                       #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                               ;
    }
    
    QString szFile = set.value("Style/File", m_szFile).toString();
    return LoadStyle(szFile);
}

int CStyle::LoadStyle(const QString &szFile)
{
    m_szFile = szFile;
    if(szFile.isEmpty())
    {
        qApp->setStyleSheet("");
        //qApp->setPalette(QPalette(QColor(Qt::gray)));
    }
    else
    {
        QFile file(szFile);
        if(file.open(QFile::ReadOnly))
        {
            qInfo(LoggerStyle) << "Load style file:" << szFile;
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
        }
        else
        {
            qDebug(LoggerStyle) << "file open file fail:" << szFile;
        }
    }
    return 0;
}

QString CStyle::GetDefaultStyle()
{
    return m_szDefaultFile;
}

QString CStyle::GetStyle()
{
    QString szFile;
    szFile = m_szDefaultFile;
    QSettings set(RabbitCommon::CDir::Instance()->GetFileUserConfigure(),
                  QSettings::IniFormat);
    szFile = set.value("Style/File", szFile).toString();
    QString szPath =  RabbitCommon::CDir::Instance()->GetDirData(true)
            + QDir::separator()
            + "style";
    if(!szFile.isEmpty()) {
        QFileInfo fi(szFile);
        szPath = fi.absoluteFilePath();
    }
    QWidget* pParent = dynamic_cast<QWidget*>(this->parent());
    szFile = RabbitCommon::CDir::GetOpenFileName(pParent, tr("Open sink"),
                 szPath,
                 tr("Style files(*.qss *.css);; All files(*.*)"));
    return szFile;
}

QString CStyle::GetStyleFile()
{
    if(!m_szFile.isEmpty())
        if(!QFile::exists(m_szFile))
            return "";
    return m_szFile;
}

} //namespace RabbitCommon
