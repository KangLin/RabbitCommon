// Copyright Copyright (c) Kang Lin studio, All Rights Reserved
// Author Kang Lin <kl222@126.com>

#include "Style.h"
#include "RabbitCommonDir.h"

#include <QSettings>
#include <QDebug>
#include <QApplication>
#include <QRegularExpression>
#include <QLoggingCategory>
#include <QDir>
#include <QColor>
#include <QPalette>
#include <QIcon>

namespace RabbitCommon {

static Q_LOGGING_CATEGORY(log, "RabbitCommon.Style")

CStyle::CStyle(QObject *parent) : QObject(parent)
{
    qDebug(log) << __FUNCTION__;
    m_szDefaultFile = QString(); /*RabbitCommon::CDir::Instance()->GetDirData(true)
            + QDir::separator()
            + "style" + QDir::separator()
            + "white.qss"; //TODO: can modify default style //*/

    m_szDefaultIconTheme = QIcon::themeName();
    if(m_szDefaultIconTheme.isEmpty()) {
        m_szDefaultIconTheme = "rabbit-black"; //TODO: can modify default icon theme
        QIcon::setThemeName(m_szDefaultIconTheme);
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
    if(!QIcon::fallbackThemeName().isEmpty())
        qDebug(log) << "Old icon fallback theme:" << QIcon::fallbackThemeName();
    //Note: the fallback icon theme must set "rabbit-*"
    m_szDefaultFallbackIconTheme = "rabbit-black"; //TODO: can modify default fallback icon theme
    QIcon::setFallbackThemeName(m_szDefaultFallbackIconTheme);
#endif
    qDebug(log)
        << "Icon theme name:" << QIcon::themeName() << "\n"
        << "Icon theme search paths:" << QIcon::themeSearchPaths() << "\n"
        #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        << "Fallback theme name:" << QIcon::fallbackThemeName() << "\n"
        #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        << "Fallback search paths:" << QIcon::fallbackSearchPaths() << "\n"
        #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
        ;
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
        QIcon::setThemeSearchPaths(
            QIcon::themeSearchPaths()
            << RabbitCommon::CDir::Instance()->GetDirIcons(true));
        QString szThemeName = QIcon::themeName();
        if(szThemeName.isEmpty())
            szThemeName = m_szDefaultIconTheme;
        QIcon::setThemeName(set.value("Style/Icon/Theme",
                                      szThemeName).toString());
        
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0) && !defined(Q_OS_WINDOWS)
        QIcon::setFallbackSearchPaths(
            QIcon::fallbackSearchPaths()
            << RabbitCommon::CDir::Instance()->GetDirIcons(true));
        QString szFallbackThemeName = QIcon::fallbackThemeName();
        if(szFallbackThemeName.isEmpty())
            szFallbackThemeName = m_szDefaultFallbackIconTheme;
        QIcon::setFallbackThemeName(set.value("Style/Icon/Theme/Fallback",
                                              szFallbackThemeName).toString());
#endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)

        qDebug(log) << __FUNCTION__ << "\n"
                    << "Icon theme name:" << QIcon::themeName() << "\n"
                    << "Icon theme search paths:" << QIcon::themeSearchPaths() << "\n"
                    #if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                    << "Fallback theme name:" << QIcon::fallbackThemeName() << "\n"
                    #endif //QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
                    #if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
                    << "Fallback search paths:" << QIcon::fallbackSearchPaths() << "\n"
                    #endif // QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
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
            qInfo(log) << "Load style file:" << szFile;
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
            qCritical(log) << "file open file fail:" << szFile;
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
#ifdef Q_OS_LINUX
    QDir d(szPath);
    if(!d.exists())
        szPath = "/usr/share/style";
#endif
    if(!szFile.isEmpty()) {
        QFileInfo fi(szFile);
        szPath = fi.absoluteFilePath();
    }
    qDebug(log) << "Path:" << szPath;
    QWidget* pParent = dynamic_cast<QWidget*>(this->parent());
    szFile = QFileDialog::getOpenFileName(pParent, tr("Open sink"),
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
